//
//  PluginChain.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#import <stdlib.h>
#import <string.h>
#import "PluginChain.h"
#include "EventLogger.h"

PluginChain newPluginChain(void) {
  PluginChain pluginChain = malloc(sizeof(PluginChainMembers));

  pluginChain->numPlugins = 0;
  pluginChain->plugins = malloc(sizeof(Plugin) * MAX_PLUGINS);

  return pluginChain;
}

static boolean _addPluginToChain(PluginChain pluginChain, Plugin plugin) {
  if(pluginChain->numPlugins + 1 >= MAX_PLUGINS) {
    logError("Could not add plugin, maximum number reached");
    return false;
  }
  else {
    pluginChain->plugins[pluginChain->numPlugins] = plugin;
    pluginChain->numPlugins++;
    return true;
  }
}

void addPluginsFromArgumentString(PluginChain pluginChain, const CharString argumentString) {
  // Expect a comma-separated string of plugins with colon separators for preset name
  // Example: plugin1:preset1name,plugin2:preset2name
  char* substringStart = argumentString;
  char* comma = strchr(argumentString, ',');
  char* endChar = argumentString + strlen(argumentString);
  CharString nameBuffer = newCharString();

  do {
    size_t substringLength = 0;
    if(comma == NULL) {
      substringLength = strlen(argumentString);
    }
    else {
      substringLength = comma - substringStart;      
    }
    strncpy(nameBuffer, substringStart, substringLength);

    // TODO: Use colon as a separator for presets to load into these plugins
    PluginType pluginType = guessPluginType(nameBuffer);
    if(pluginType != PLUGIN_TYPE_INVALID) {
      Plugin plugin = newPlugin(pluginType, nameBuffer);
      _addPluginToChain(pluginChain, plugin);
    }

    if(comma == NULL) {
      break;
    }
    else {
      substringStart = comma + 1;
      comma = strchr(comma + 1, ',');
    }
  } while(substringStart < endChar);

  free(nameBuffer);
}

void initializePluginChain(PluginChain pluginChain) {
  for(int i = 0; i < pluginChain->numPlugins; i++) {
    Plugin plugin = pluginChain->plugins[i];
    plugin->open(plugin);
  }
}

void process(PluginChain pluginChain, SampleBuffer inBuffer, SampleBuffer outBuffer) {
  
}

void freePluginChain(PluginChain pluginChain) {
  for(int i = 0; i < pluginChain->numPlugins; i++) {
    Plugin plugin = pluginChain->plugins[i];
    freePlugin(plugin);
  }
  free(pluginChain);
}
