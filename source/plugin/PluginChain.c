//
// PluginChain.c - MrsWatson
// Created by Nik Reiman on 1/3/12.
// Copyright (c) 2012 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging/EventLogger.h"
#include "plugin/PluginChain.h"
#include "audio/AudioSettings.h"

ReturnCodes pluginInitialize(Plugin, PluginPreset);

PluginChain newPluginChain(void) {
  PluginChain pluginChain = (PluginChain)malloc(sizeof(PluginChainMembers));

  pluginChain->numPlugins = 0;
  pluginChain->plugins = (Plugin*)malloc(sizeof(Plugin) * MAX_PLUGINS);
  pluginChain->presets = (PluginPreset*)malloc(sizeof(PluginPreset) * MAX_PLUGINS);
  pluginChain->audioTimers = (TaskTimer*)malloc(sizeof(TaskTimer) * MAX_PLUGINS);
  pluginChain->midiTimers = (TaskTimer*)malloc(sizeof(TaskTimer) * MAX_PLUGINS);

  return pluginChain;
}

boolByte pluginChainAppend(PluginChain self, Plugin plugin, PluginPreset preset) {
  if(plugin == NULL) {
    return false;
  }
  else if(self->numPlugins + 1 >= MAX_PLUGINS) {
    logError("Could not add plugin '%s', maximum number reached", plugin->pluginName->data);
    return false;
  }
  else if(self->numPlugins > 0 && plugin->pluginType == PLUGIN_TYPE_INSTRUMENT) {
    logError("Instrument plugin '%s' must be first in the chain", plugin->pluginName->data);
    return false;
  }
  else {
    self->plugins[self->numPlugins] = plugin;
    self->presets[self->numPlugins] = preset;
    pluginInitialize(plugin, preset);
    self->audioTimers[self->numPlugins] = newTaskTimer(plugin->pluginName, "Audio Processing");
    self->midiTimers[self->numPlugins] = newTaskTimer(plugin->pluginName, "MIDI Processing");
    self->numPlugins++;
    return true;
  }
}

boolByte pluginChainAddFromArgumentString(PluginChain pluginChain, const CharString argumentString, const CharString userSearchPath) {
  // Expect a semicolon-separated string of plugins with comma separators for preset names
  // Example: plugin1,preset1name;plugin2,preset2name
  char* substringStart;
  char* pluginSeparator;
  char* endChar;
  CharString pluginNameBuffer = NULL;
  CharString presetNameBuffer = NULL;
  char* presetSeparator;
  PluginPreset preset;
  Plugin plugin;
  size_t substringLength;

  if(charStringIsEmpty(argumentString)) {
    logWarn("Plugin chain string is empty");
    return false;
  }
  substringStart = argumentString->data;
  pluginSeparator = strchr(argumentString->data, CHAIN_STRING_PLUGIN_SEPARATOR);
  endChar = argumentString->data + strlen(argumentString->data);

  do {
    if(pluginSeparator == NULL) {
      substringLength = strlen(argumentString->data);
    }
    else {
      substringLength = pluginSeparator - substringStart;
    }
    pluginNameBuffer = newCharString();
    strncpy(pluginNameBuffer->data, substringStart, substringLength);

    // Look for the separator for presets to load into these plugins
    presetNameBuffer = newCharString();
    presetSeparator = strchr(pluginNameBuffer->data, CHAIN_STRING_PROGRAM_SEPARATOR);
    if(presetSeparator != NULL) {
      // Null-terminate this string to force it to end, then extract preset name from next char
      *presetSeparator = '\0';
      strncpy(presetNameBuffer->data, presetSeparator + 1, strlen(presetSeparator + 1));
    }

    // Find preset for this plugin (if given)
    preset = NULL;
    if(strlen(presetNameBuffer->data) > 0) {
      logInfo("Opening preset '%s' for plugin", presetNameBuffer->data);
      preset = pluginPresetFactory(presetNameBuffer);
    }

    // Guess the plugin type from the file extension, search root, etc.
    plugin = pluginFactory(pluginNameBuffer, userSearchPath);
    if(plugin != NULL) {
      if(!pluginChainAppend(pluginChain, plugin, preset)) {
        logError("Plugin '%s' could not be added to the chain", pluginNameBuffer->data);
        free(pluginNameBuffer);
        free(presetNameBuffer);
        return false;
      }
    }

    if(pluginSeparator == NULL) {
      break;
    }
    else {
      substringStart = pluginSeparator + 1;
      pluginSeparator = strchr(pluginSeparator + 1, CHAIN_STRING_PLUGIN_SEPARATOR);
    }
  } while(substringStart < endChar);

  freeCharString(pluginNameBuffer);
  freeCharString(presetNameBuffer);
  return true;
}

static boolByte _loadPresetForPlugin(Plugin plugin, PluginPreset preset) {
  if(pluginPresetIsCompatibleWith(preset, plugin)) {
    if(!preset->openPreset(preset)) {
      logError("Could not open preset '%s'", preset->presetName->data);
      return false;
    }
    if(!preset->loadPreset(preset, plugin)) {
      logError("Could not load preset '%s' in plugin '%s'", preset->presetName->data, plugin->pluginName->data);
      return false;
    }
    logInfo("Loaded preset '%s' in plugin '%s'", preset->presetName->data, plugin->pluginName->data);
    return true;
  }
  else {
    logError("Preset '%s' is not a compatible format for plugin", preset->presetName->data);
    return false;
  }
}

ReturnCodes pluginInitialize(Plugin plugin, PluginPreset preset) {
  if(!plugin->openPlugin(plugin)) {
    logError("Plugin '%s' could not be opened", plugin->pluginName->data);
    return RETURN_CODE_PLUGIN_ERROR;
  }
  else {
    if(plugin->pluginType == PLUGIN_TYPE_UNKNOWN) {
      logError("Plugin '%s' has unknown type; It was probably not loaded correctly", plugin->pluginName->data);
      return RETURN_CODE_PLUGIN_ERROR;
    }
    else if(plugin->pluginType == PLUGIN_TYPE_UNSUPPORTED) {
      logError("Plugin '%s' is of unsupported type", plugin->pluginName->data);
      return RETURN_CODE_PLUGIN_ERROR;
    }

    if(preset != NULL) {
      if(!_loadPresetForPlugin(plugin, preset)) {
        return RETURN_CODE_INVALID_ARGUMENT;
      }
    }
  }

  return RETURN_CODE_SUCCESS;
}

void pluginChainInspect(PluginChain pluginChain) {
  Plugin plugin;
  unsigned int i;
  for(i = 0; i < pluginChain->numPlugins; i++) {
    plugin = pluginChain->plugins[i];
    plugin->displayInfo(plugin);
  }
}

void pluginChainPrepareForProcessing(PluginChain self) {
  Plugin plugin;
  unsigned int i;
  for(i = 0; i < self->numPlugins; i++) {
    plugin = self->plugins[i];
    plugin->prepareForProcessing(plugin);
  }
}

int pluginChainGetMaximumTailTimeInMs(PluginChain pluginChain) {
  Plugin plugin;
  int tailTime;
  int maxTailTime = 0;
  unsigned int i;
  for(i = 0; i < pluginChain->numPlugins; i++) {
    plugin = pluginChain->plugins[i];
    tailTime = plugin->getSetting(plugin, PLUGIN_SETTING_TAIL_TIME_IN_MS);
    if(tailTime > maxTailTime) {
      maxTailTime = tailTime;
    }
  }
  return maxTailTime;
}

typedef struct {
  Plugin plugin;
  boolByte success;
} _PluginChainSetParameterPassData;

void _pluginChainSetParameter(void* item, void* userData) {
  // Expect that the linked list contains CharStrings, single that is what is
  // being given from the command line.
  char* parameterValue = (char*)item;
  _PluginChainSetParameterPassData* passData = (_PluginChainSetParameterPassData*)userData;
  Plugin plugin = passData->plugin;
  char* comma = NULL;
  int index;
  float value;

  // If a previous attempt to set a parameter failed, then return right away
  // since this method will return false anyways.
  if(!passData->success) {
    return;
  }

  // TODO: Need a "pair" type, this string parsing is done several times in the codebase
  comma = strchr(parameterValue, ',');
  if(comma == NULL) {
    logError("Malformed parameter string, see --help parameter for usage");
    return;
  }
  *comma = '\0';
  index = (int)strtod(parameterValue, NULL);
  value = (float)strtod(comma + 1, NULL);
  logDebug("Set parameter %d to %f", index, value);
  passData->success = plugin->setParameter(plugin, index, value);
}

boolByte pluginChainSetParameters(PluginChain self, const LinkedList parameters) {
  _PluginChainSetParameterPassData passData;
  passData.plugin = self->plugins[0];
  passData.success = true;
  logDebug("Setting parameters on head plugin in chain");
  linkedListForeach(parameters, _pluginChainSetParameter, &passData);
  return passData.success;
}

void pluginChainProcessAudio(PluginChain pluginChain, SampleBuffer inBuffer, SampleBuffer outBuffer) {
  Plugin plugin;
  unsigned int pluginInputs, pluginOutputs;
  unsigned int i;
  double processingTimeInMs;
  const double maxProcessingTimeInMs = inBuffer->blocksize * 1000.0 / getSampleRate();

  for(i = 0; i < pluginChain->numPlugins; i++) {
    sampleBufferClear(outBuffer);

    plugin = pluginChain->plugins[i];
    logDebug("Processing audio with plugin '%s'", plugin->pluginName->data);
    pluginInputs = plugin->getSetting(plugin, PLUGIN_NUM_INPUTS);
    if(inBuffer->numChannels < pluginInputs) {
      logDebug("Expanding input source from %d -> %d channels", inBuffer->numChannels, pluginInputs);
      sampleBufferResize(inBuffer, pluginInputs, true);
    }
    pluginOutputs = plugin->getSetting(plugin, PLUGIN_NUM_OUTPUTS);
    if(outBuffer->numChannels < pluginOutputs) {
      logDebug("Expanding output source from %d -> %d channels", outBuffer->numChannels, pluginOutputs);
      sampleBufferResize(outBuffer, pluginOutputs, false);
    }
    taskTimerStart(pluginChain->audioTimers[i]);
    plugin->processAudio(plugin, inBuffer, outBuffer);
    processingTimeInMs = taskTimerStop(pluginChain->audioTimers[i]);
    if(processingTimeInMs > maxProcessingTimeInMs) {
      logWarn("Possible dropout! Plugin '%s' spent %dms processing time (%dms max)",
        plugin->pluginName->data, (int)processingTimeInMs, (int)maxProcessingTimeInMs);
    }
    else {
      logDebug("Plugin '%s' spent %dms processing (%d%% effective CPU usage)",
        plugin->pluginName->data, (int)processingTimeInMs,
        (int)(processingTimeInMs / maxProcessingTimeInMs));
    }
    
    // If this is not the last plugin in the chain, then copy the output of this plugin
    // back to the input for the next one in the chain.
    if(i + 1 < pluginChain->numPlugins) {
      sampleBufferCopy(inBuffer, outBuffer);
    }
  }
}

void pluginChainProcessMidi(PluginChain pluginChain, LinkedList midiEvents) {
  Plugin plugin;
  if(midiEvents->item != NULL) {
    logDebug("Processing plugin chain MIDI events");
    // Right now, we only process MIDI in the first plugin in the chain
    // TODO: Is this really the correct behavior? How do other sequencers do it?
    plugin = pluginChain->plugins[0];
    taskTimerStart(pluginChain->midiTimers[0]);
    plugin->processMidiEvents(plugin, midiEvents);
    taskTimerStop(pluginChain->midiTimers[0]);
  }
}

void pluginChainShutdown(PluginChain pluginChain) {
  Plugin plugin;
  unsigned int i;
  for(i = 0; i < pluginChain->numPlugins; i++) {
    plugin = pluginChain->plugins[i];
    logInfo("Closing plugin '%s'", plugin->pluginName->data);
    plugin->closePlugin(plugin);
  }
}

void freePluginChain(PluginChain pluginChain) {
  unsigned int i;

  for(i = 0; i < pluginChain->numPlugins; i++) {
    freePluginPreset(pluginChain->presets[i]);
    freePlugin(pluginChain->plugins[i]);
    freeTaskTimer(pluginChain->audioTimers[i]);
    freeTaskTimer(pluginChain->midiTimers[i]);
  }

  free(pluginChain->presets);
  free(pluginChain->plugins);
  free(pluginChain->audioTimers);
  free(pluginChain->midiTimers);
  free(pluginChain);
}
