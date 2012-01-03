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
#import "CharStringList.h"

static CharStringList _newDefaultPluginLocationArray(PlatformType platformType) {
  CharStringList locations = newCharStringList();
  CharString locationBuffer = newCharString();

  switch(platformType) {
    case PLATFORM_WINDOWS:
      // TODO: Yeah, whatever
      break;
    case PLATFORM_MACOSX:
      snprintf(locationBuffer, STRING_LENGTH, "/Library/Audio/Plug-Ins/VST");
      addItemToStringList(locations, locationBuffer);
      snprintf(locationBuffer, STRING_LENGTH, "%s/Library/Audio/Plug-Ins/VST", getenv("HOME"));
      addItemToStringList(locations, locationBuffer);
      break;
    case PLATFORM_UNSUPPORTED:
    default:
      logCritical("Unsupported platform, sorry!");
      break;
  }

  free(locationBuffer);
  return locations;
}

static const char*_getVst2xPlatformExtension(void) {
  PlatformType platformType = getPlatformType();
  switch(platformType) {
    case PLATFORM_MACOSX:
      return "vst";
    case PLATFORM_WINDOWS:
      return "dll";
    default:
      return "";
  }
}

bool locateVst2xPlugin(const CharString pluginName, CharString outLocation) {
  CharStringList pluginLocations = _newDefaultPluginLocationArray(getPlatformType());
  if(pluginLocations->item == NULL) {
    return false;
  }

  bool result = false;
  CharString pluginSearchPath = newCharString();
  CharStringListIterator iterator = pluginLocations;
  while(iterator->nextItem != NULL) {
    snprintf(pluginSearchPath, STRING_LENGTH, "%s%c%s.%s", iterator->item, PATH_DELIMITER, pluginName, _getVst2xPlatformExtension());
    if(fileExists(pluginSearchPath)) {
      strncpy(outLocation, pluginSearchPath, STRING_LENGTH);
      result = true;
      break;
    }
    iterator = iterator->nextItem;
  }

  free(pluginSearchPath);
  freeCharStringList(pluginLocations);

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

  PluginVst2xData extraData = malloc(sizeof(PluginVst2xDataMembers));
  plugin->extraData = extraData;

  return plugin;
}
