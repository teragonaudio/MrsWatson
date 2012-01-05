//
//  PluginChain.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
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

  return pluginChain;
}

static boolean _addPluginToChain(PluginChain pluginChain, Plugin plugin) {
  if(pluginChain->numPlugins + 1 >= MAX_PLUGINS) {
    logError("Could not add plugin '%s', maximum number reached", plugin->pluginName->data);
    return false;
  }
  else {
    pluginChain->plugins[pluginChain->numPlugins] = plugin;
    pluginChain->numPlugins++;
    return true;
  }
}

boolean addPluginsFromArgumentString(PluginChain pluginChain, const CharString argumentString) {
  // Expect a comma-separated string of plugins with colon separators for preset name
  // Example: plugin1:preset1name,plugin2:preset2name
  char* substringStart = argumentString->data;
  char* comma = strchr(argumentString->data, ',');
  char* endChar = argumentString->data + strlen(argumentString->data);
  CharString nameBuffer = newCharString();

  do {
    size_t substringLength;
    if(comma == NULL) {
      substringLength = strlen(argumentString->data);
    }
    else {
      substringLength = comma - substringStart;      
    }
    strncpy(nameBuffer->data, substringStart, substringLength);

    // TODO: Use colon as a separator for presets to load into these plugins
    PluginInterfaceType pluginType = guessPluginInterfaceType(nameBuffer);
    if(pluginType != PLUGIN_TYPE_INVALID) {
      Plugin plugin = newPlugin(pluginType, nameBuffer);
      if(!_addPluginToChain(pluginChain, plugin)) {
        logError("Plugin chain could not be constructed");
        return false;
      }
    }

    if(comma == NULL) {
      break;
    }
    else {
      substringStart = comma + 1;
      comma = strchr(comma + 1, ',');
    }
  } while(substringStart < endChar);

  freeCharString(nameBuffer);
  return true;
}

boolean initializePluginChain(PluginChain pluginChain) {
  for(int i = 0; i < pluginChain->numPlugins; i++) {
    Plugin plugin = pluginChain->plugins[i];
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
    }
  }

  return true;
}

void displayPluginInfo(PluginChain pluginChain) {
  for(int i = 0; i < pluginChain->numPlugins; i++) {
    Plugin plugin = pluginChain->plugins[i];
    plugin->displayPluginInfo(plugin);
  }
}

void process(PluginChain pluginChain, SampleBuffer inBuffer, SampleBuffer outBuffer) {
  logDebug("Processing plugin chain");
  for(int i = 0; i < pluginChain->numPlugins; i++) {
    clearSampleBuffer(outBuffer);
    Plugin plugin = pluginChain->plugins[i];
    logDebug("Processing plugin '%s'", plugin->pluginName->data);
    plugin->process(plugin, inBuffer, outBuffer);

    // If this is not the last plugin in the chain, then copy the output of this plugin
    // back to the input for the next one in the chain.
    if(i + 1 < pluginChain->numPlugins) {
      copySampleBuffers(inBuffer, outBuffer);
    }
  }
}

void freePluginChain(PluginChain pluginChain) {
  for(int i = 0; i < pluginChain->numPlugins; i++) {
    Plugin plugin = pluginChain->plugins[i];
    logInfo("Closing plugin '%s'", plugin->pluginName->data);
    freePlugin(plugin);
  }
  free(pluginChain);
}
