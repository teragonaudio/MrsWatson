//
//  PluginVst2x.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#import <stdlib.h>
#import <string.h>
#import "PluginVst2x.h"
#import "PlatformInfo.h"
#import "EventLogger.h"

static CharString* _newDefaultPluginLocationArray(PlatformType platformType) {
  switch(platformType) {
    case PLATFORM_WINDOWS:
      // TODO: Yeah, whatever
      return NULL;
    case PLATFORM_MACOSX:
    {
      CharString* locations = malloc(STRING_LENGTH * 2);
      for(int i = 0; i < 2; i++) {
        locations[i] = newCharString();
      }
      snprintf(locations[0], STRING_LENGTH, "/Library/Audio/Plug-Ins/VST");
      snprintf(locations[1], STRING_LENGTH, "%s/Library/Audio/Plug-Ins/VST", getenv("HOME"));
      return locations;
    }
    case PLATFORM_UNSUPPORTED:
      logCritical("Unsupported platform, sorry!");
      return NULL;
    default:
      return NULL;
  }
}

bool locateVst2xPlugin(const CharString pluginName, CharString outLocation) {
  // TODO: Meh, should probably be a linked list or something here
  CharString* pluginLocations = _newDefaultPluginLocationArray(getPlatformType());
  if(pluginLocations == NULL) {
    return false;
  }

  bool result = false;
  int numLocations = sizeof(pluginLocations) / STRING_LENGTH;
  CharString pluginSearchPath = newCharString();
  for(int i = 0; i < numLocations; i++) {
    snprintf(pluginSearchPath, STRING_LENGTH, "%s%c%s", pluginLocations[i], PATH_DELIMITER, pluginName);
    if(fileExists(pluginSearchPath)) {
      strncpy(outLocation, pluginSearchPath, STRING_LENGTH);
      result = true;
      break;
    }
  }
  free(pluginSearchPath);

  for(int i = 0; i < numLocations; i++) {
    free(pluginLocations[i]);
  }
  free(pluginLocations);

  return result;
}

static bool _openVst2xPlugin(void* pluginPtr, const CharString pluginName) {
  return false;
}

static void _processVst2xPlugin(void* pluginPtr, SampleBuffer sampleBuffer) {
  
}

static void _freeVst2xPluginData(void* pluginPtr) {
  Plugin plugin = pluginPtr;
  PluginVst2xData data = plugin->extraData;
  // TODO: Free, close plugin
  free(data);
}

Plugin newPluginVst2x(void) {
  Plugin plugin = malloc(sizeof(PluginMembers));

  plugin->pluginType = PLUGIN_TYPE_VST_2X;
  plugin->pluginName = newCharString();

  plugin->open = _openVst2xPlugin;
  plugin->process = _processVst2xPlugin;
  plugin->freePluginData = _freeVst2xPluginData;

  plugin->extraData = malloc(sizeof(PluginVst2xDataMembers));
  return plugin;
}
