//
//  Plugin.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "CharString.h"
#include "SampleBuffer.h"

#ifndef MrsWatson_Plugin_h
#define MrsWatson_Plugin_h

typedef enum {
  PLUGIN_TYPE_INVALID,
  PLUGIN_TYPE_VST_2X,
  NUM_PLUGIN_INTERFACE_TYPES
} PluginInterfaceType;

typedef enum {
  PLUGIN_TYPE_UNKNOWN,
  PLUGIN_TYPE_UNSUPPORTED,
  PLUGIN_TYPE_EFFECT,
  PLUGIN_TYPE_INSTRUMENT,
  NUM_PLUGIN_TYPES
} PluginType;

typedef boolean (*OpenPluginFunc)(void*);
typedef void (*DisplayPluginInfoFunc)(void*);
typedef void (*PluginProcessFunc)(void*, SampleBuffer, SampleBuffer);
typedef void (*FreePluginDataFunc)(void*);

typedef struct {
  PluginInterfaceType interfaceType;
  PluginType pluginType;
  CharString pluginName;

  OpenPluginFunc open;
  DisplayPluginInfoFunc displayPluginInfo;
  PluginProcessFunc process;
  FreePluginDataFunc freePluginData;

  void* extraData;
} PluginMembers;

typedef PluginMembers* Plugin;

PluginInterfaceType guessPluginInterfaceType(CharString interfaceTypeName);
Plugin newPlugin(PluginInterfaceType pluginInterfaceType, const CharString pluginName);
void freePlugin(Plugin plugin);

#endif
