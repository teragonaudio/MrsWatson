//
//  Plugin.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#import "CharString.h"
#import "SampleBuffer.h"

#ifndef MrsWatson_Plugin_h
#define MrsWatson_Plugin_h

typedef enum {
  PLUGIN_TYPE_INVALID,
  PLUGIN_TYPE_VST_2X,
} PluginType;

typedef boolean (*OpenPluginFunc)(void*);
typedef void (*PluginProcessFunc)(void*, SampleBuffer);
typedef void (*FreePluginDataFunc)(void*);

typedef struct {
  PluginType pluginType;
  CharString pluginName;

  OpenPluginFunc open;
  PluginProcessFunc process;
  FreePluginDataFunc freePluginData;

  void* extraData;
} PluginMembers;

typedef PluginMembers* Plugin;

PluginType guessPluginType(CharString pluginName);
Plugin newPlugin(PluginType pluginType, const CharString pluginName);
void freePlugin(Plugin plugin);

#endif
