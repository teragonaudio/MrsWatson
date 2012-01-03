//
//  Plugin.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#import <stdlib.h>
#import "Plugin.h"
#import "EventLogger.h"

// Do NOT include PluginVst2x.h from this file, otherwise we start mixing too much C/C++ code
// and things become a real mess. Instead, we simply extern the functions needed in that file
// for use here. Yes, this is a bit redundant, but otherwise the entire project starts being
// "infected" by C++.
extern bool vst2xPluginExists(const CharString pluginName);
extern Plugin newPluginVst2x(const CharString pluginName);

PluginType guessPluginType(CharString pluginName) {
  PluginType pluginType = PLUGIN_TYPE_INVALID;
  logDebug("Trying to find plugin '%s'", pluginName);

  if(vst2xPluginExists(pluginName)) {
    logDebug("Plugin is VST 2.x");
    pluginType = PLUGIN_TYPE_VST_2X;
  }
  else {
    logError("Plugin '%s' could not be found", pluginName);
  }

  return pluginType;
}

Plugin newPlugin(PluginType pluginType, const CharString pluginName) {
  switch(pluginType) {
    case PLUGIN_TYPE_VST_2X:
      return newPluginVst2x(pluginName);
    case PLUGIN_TYPE_INVALID:
    default:
      logError("Plugin type not supported");
      return NULL;
  }
}

void freePlugin(Plugin plugin) {
  free(plugin->pluginName);
  plugin->freePluginData(plugin->extraData);
  free(plugin);
}
