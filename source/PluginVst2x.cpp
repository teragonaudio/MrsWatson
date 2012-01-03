//
//  PluginVst2x.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include "PluginVst2x.h"
#include "PlatformInfo.h"
#include "EventLogger.h"
#include "CharStringList.h"
}

#include "aeffectx.h"

VstIntPtr VSTCALLBACK vst2xPluginHostCallback(AEffect *effect, VstInt32 opcode, VstInt32 index, VstInt32 value, void *ptr, float opt);

typedef AEffect* (*Vst2xPluginEntryFuncPtr)(audioMasterCallback host);
typedef VstIntPtr (*Vst2xPluginDispatcherFuncPtr)(AEffect *effect, VstInt32 opCode, VstInt32 index, VstInt32 value, void *ptr, float opt);
typedef float (*Vst2xPluginGetParameterFuncPtr)(AEffect *effect, VstInt32 index);
typedef void (*Vst2xPluginSetParameterFuncPtr)(AEffect *effect, VstInt32 index, float value);
typedef VstInt32 (*Vst2xPluginProcessEventsFuncPtr)(struct VstEvents* events);
typedef void (*Vst2xPluginProcessFuncPtr)(AEffect* effect, float** inputs, float** outputs, VstInt32 sampleFrames);

extern "C" {
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

static CharString _newVst2xPluginAbsolutePath(const CharString pluginName) {
  CharStringList pluginLocations = _newDefaultPluginLocationArray(getPlatformType());
  if(pluginLocations->item == NULL) {
    return NULL;
  }

  CharString result = NULL;
  CharString pluginSearchPath = newCharString();
  CharStringListIterator iterator = pluginLocations;
  while(iterator->nextItem != NULL) {
    snprintf(pluginSearchPath, STRING_LENGTH, "%s%c%s.%s", iterator->item, PATH_DELIMITER, pluginName, _getVst2xPlatformExtension());
    if(fileExists(pluginSearchPath)) {
      result = pluginSearchPath;
    }
    iterator = (CharStringListIterator)iterator->nextItem;
  }

  free(pluginSearchPath);
  freeCharStringList(pluginLocations);
  return result;
}

boolean vst2xPluginExists(const CharString pluginName) {
  CharString pluginLocation = _newVst2xPluginAbsolutePath(pluginName);
  boolean result = !isStringEmpty(pluginLocation);
  free(pluginLocation);
  return result;
}

static boolean _openVst2xPlugin(void* pluginPtr) {
  Plugin plugin = (Plugin)pluginPtr;
  CharString pluginLocation = _newVst2xPluginAbsolutePath(plugin->pluginName);
  free(pluginLocation);
  return false;
}

static void _processVst2xPlugin(void* pluginPtr, SampleBuffer sampleBuffer) {
  
}

static void _freeVst2xPluginData(void* pluginPtr) {
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)(plugin->extraData);
  // TODO: Free, close plugin
  free(data);
}

Plugin newPluginVst2x(const CharString pluginName) {
  Plugin plugin = (Plugin)malloc(sizeof(PluginMembers));

  plugin->pluginType = PLUGIN_TYPE_VST_2X;
  plugin->pluginName = newCharString();
  strncpy(plugin->pluginName, pluginName, STRING_LENGTH);

  plugin->open = _openVst2xPlugin;
  plugin->process = _processVst2xPlugin;
  plugin->freePluginData = _freeVst2xPluginData;

  PluginVst2xData extraData = (PluginVst2xData)malloc(sizeof(PluginVst2xDataMembers));
  plugin->extraData = extraData;

  return plugin;
}
}
