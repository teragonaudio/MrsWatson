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
#import "PluginVst2x.h"
#import "EventLogger.h"

PluginType guessPluginType(CharString pluginName) {
  PluginType pluginType = PLUGIN_TYPE_INVALID;
  CharString pluginFullPath = newCharString();
  logDebug("Trying to find plugin '%s'", pluginName);

  if(locateVst2xPlugin(pluginName, pluginFullPath)) {
    logDebug("Plugin is VST 2.x");
    pluginType = PLUGIN_TYPE_VST_2X;
  }
  else {
    logError("Plugin '%s' could not be found", pluginName);
  }

  free(pluginFullPath);
  return pluginType;
}

Plugin newPlugin(PluginType pluginType) {
  switch(pluginType) {
    case PLUGIN_TYPE_VST_2X:
      return newPluginVst2x();
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
