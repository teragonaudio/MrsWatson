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
#include "PluginChain.h"
#include "EventLogger.h"

PluginChain newPluginChain(void) {
  PluginChain pluginChain = malloc(sizeof(PluginChainMembers));

  pluginChain->numPlugins = 0;
  pluginChain->plugins = malloc(sizeof(Plugin) * MAX_PLUGINS);
  pluginChain->presets = malloc(sizeof(PluginPreset) * MAX_PLUGINS);

  return pluginChain;
}

static boolByte _addPluginToChain(PluginChain pluginChain, Plugin plugin, PluginPreset preset) {
  if(pluginChain->numPlugins + 1 >= MAX_PLUGINS) {
    logError("Could not add plugin '%s', maximum number reached", plugin->pluginName->data);
    return false;
  }
  else {
    pluginChain->plugins[pluginChain->numPlugins] = plugin;
    pluginChain->presets[pluginChain->numPlugins] = preset;
    pluginChain->numPlugins++;
    return true;
  }
}

boolByte addPluginsFromArgumentString(PluginChain pluginChain, const CharString argumentString, const CharString userSearchPath) {
  // Expect a semicolon-separated string of plugins with comma separators for preset names
  // Example: plugin1,preset1name;plugin2,preset2name
  char* substringStart = argumentString->data;
  char* pluginSeparator = strchr(argumentString->data, CHAIN_STRING_PLUGIN_SEPARATOR);
  char* endChar = argumentString->data + strlen(argumentString->data);
  CharString pluginNameBuffer = newCharString();
  CharString presetNameBuffer;
  char* presetSeparator;
  PluginPreset preset = NULL;
  PluginPresetType presetType;
  Plugin plugin;
  size_t substringLength;
  CharString pluginLocationBuffer;
  PluginInterfaceType pluginType;

  do {
    if(pluginSeparator == NULL) {
      substringLength = strlen(argumentString->data);
    }
    else {
      substringLength = pluginSeparator - substringStart;
    }
    strncpy(pluginNameBuffer->data, substringStart, substringLength);

    // Use colon as a separator for presets to load into these plugins
    presetNameBuffer = newCharString();
    presetSeparator = strchr(pluginNameBuffer->data, CHAIN_STRING_PROGRAM_SEPARATOR);
    if(presetSeparator != NULL) {
      // Null-terminate this string to force it to end, then extract preset name from next char
      *presetSeparator = '\0';
      strncpy(presetNameBuffer->data, presetSeparator + 1, strlen(presetSeparator + 1));
    }

    // Find preset for this plugin (if given)
    if(strlen(presetNameBuffer->data) > 0) {
      logInfo("Opening preset '%s' for plugin", presetNameBuffer->data);
      presetType = guessPluginPresetType(presetNameBuffer);
      if(presetType != PRESET_TYPE_INVALID) {
        preset = newPluginPreset(presetType, presetNameBuffer);
      }
    }
    freeCharString(presetNameBuffer);

    // Guess the plugin type from the file extension, search root, etc.
    pluginLocationBuffer = newCharString();
    pluginType = guessPluginInterfaceType(pluginNameBuffer, userSearchPath, pluginLocationBuffer);
    if(pluginType != PLUGIN_TYPE_INVALID) {
      plugin = newPlugin(pluginType, pluginNameBuffer, pluginLocationBuffer);
      if(!_addPluginToChain(pluginChain, plugin, preset)) {
        logError("Plugin chain could not be constructed");
        return false;
      }
    }
    freeCharString(pluginLocationBuffer);

    if(pluginSeparator == NULL) {
      break;
    }
    else {
      substringStart = pluginSeparator + 1;
      pluginSeparator = strchr(pluginSeparator + 1, CHAIN_STRING_PLUGIN_SEPARATOR);
    }
  } while(substringStart < endChar);

  freeCharString(pluginNameBuffer);
  return true;
}

static boolByte _loadPresetForPlugin(Plugin plugin, PluginPreset preset) {
  if(isPresetCompatibleWithPlugin(preset, plugin)) {
    if(!preset->openPreset(preset)) {
      logError("Could not open preset '%s'", preset->presetName->data);
      return false;
    }
    if(!preset->loadPreset(preset, plugin)) {
      logError("Could not load preset '%s'", preset->presetName->data);
      return false;
    }
    logInfo("Loaded preset '%s' to plugin '%s'", preset->presetName->data, plugin->pluginName->data);
    return true;
  }
  else {
    logError("Preset '%s' is not a compatible format for plugin", preset->presetName->data);
    return false;
  }
}

boolByte initializePluginChain(PluginChain pluginChain) {
  Plugin plugin;
  PluginPreset preset;
  int i;

  for(i = 0; i < pluginChain->numPlugins; i++) {
    plugin = pluginChain->plugins[i];
    if(!plugin->open(plugin)) {
      logError("Plugin '%s' could not be opened", plugin->pluginName->data);
      return false;
    }
    else {
      if(i > 1 && plugin->pluginType == PLUGIN_TYPE_INSTRUMENT) {
        logError("Instrument plugin '%s' must be first in the chain", plugin->pluginName->data);
        return false;
      }
      else if(plugin->pluginType == PLUGIN_TYPE_UNKNOWN) {
        logError("Plugin '%s' has unknown type; It was probably not loaded correctly", plugin->pluginName->data);
        return false;
      }
      else if(plugin->pluginType == PLUGIN_TYPE_UNSUPPORTED) {
        logError("Plugin '%s' is of unsupported type", plugin->pluginName->data);
        return false;
      }

      preset = pluginChain->presets[i];
      if(preset != NULL) {
        if(!_loadPresetForPlugin(plugin, preset)) {
          logError("Could not load preset '%s' for plugin '%s'", preset->presetName->data, plugin->pluginName->data);
          return false;
        }
      }
    }
  }

  return true;
}

void displayPluginInfo(PluginChain pluginChain) {
  Plugin plugin;
  int i;
  for(i = 0; i < pluginChain->numPlugins; i++) {
    plugin = pluginChain->plugins[i];
    plugin->displayInfo(plugin);
  }
}

int getMaximumTailTimeInMs(PluginChain pluginChain) {
  Plugin plugin;
  int tailTime;
  int maxTailTime = 0;
  int i;
  for(i = 0; i < pluginChain->numPlugins; i++) {
    plugin = pluginChain->plugins[i];
    tailTime = plugin->getSetting(plugin, PLUGIN_SETTING_TAIL_TIME_IN_MS);
    if(tailTime > maxTailTime) {
      maxTailTime = tailTime;
    }
  }
  return maxTailTime;
}

void processPluginChainAudio(PluginChain pluginChain, SampleBuffer inBuffer, SampleBuffer outBuffer, TaskTimer taskTimer) {
  Plugin plugin;
  int i;

  for(i = 0; i < pluginChain->numPlugins; i++) {
    clearSampleBuffer(outBuffer);
    plugin = pluginChain->plugins[i];
    logDebug("Processing audio with plugin '%s'", plugin->pluginName->data);
    startTimingTask(taskTimer, i);
    plugin->processAudio(plugin, inBuffer, outBuffer);
    // TODO: Last task ID is the host, but this is a bit hacky
    startTimingTask(taskTimer, taskTimer->numTasks - 1);

    // If this is not the last plugin in the chain, then copy the output of this plugin
    // back to the input for the next one in the chain.
    if(i + 1 < pluginChain->numPlugins) {
      copySampleBuffers(inBuffer, outBuffer);
    }
  }
}

void processPluginChainMidiEvents(PluginChain pluginChain, LinkedList midiEvents, TaskTimer taskTimer) {
  Plugin plugin;
  if(midiEvents->item != NULL) {
    logDebug("Processing plugin chain MIDI events");
    // Right now, we only process MIDI in the first plugin in the chain
    // TODO: Is this really the correct behavior? How do other sequencers do it?
    plugin = pluginChain->plugins[0];
    startTimingTask(taskTimer, 0);
    plugin->processMidiEvents(plugin, midiEvents);
  }
}

void closePluginChain(PluginChain pluginChain) {
  Plugin plugin;
  int i;
  for(i = 0; i < pluginChain->numPlugins; i++) {
    plugin = pluginChain->plugins[i];
    logInfo("Closing plugin '%s'", plugin->pluginName->data);
    plugin->closePlugin(plugin);
  }
}

void freePluginChain(PluginChain pluginChain) {
  Plugin plugin;
  PluginPreset preset;
  int i;

  for(i = 0; i < pluginChain->numPlugins; i++) {
    plugin = pluginChain->plugins[i];
    freePlugin(plugin);
  }
  free(pluginChain->plugins);

  for(i = 0; i < pluginChain->numPlugins; i++) {
    preset = pluginChain->presets[i];
    if(preset != NULL) {
      preset->freePresetData(preset);
    }
  }
  free(pluginChain->presets);

  free(pluginChain);
}
