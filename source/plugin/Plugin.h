//
// Plugin.h - MrsWatson
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

#ifndef MrsWatson_Plugin_h
#define MrsWatson_Plugin_h

#include "audio/SampleBuffer.h"
#include "base/CharString.h"
#include "base/LinkedList.h"

typedef enum {
  PLUGIN_TYPE_INVALID,
  PLUGIN_TYPE_VST_2X,
  PLUGIN_TYPE_INTERNAL,
  NUM_PLUGIN_INTERFACE_TYPES
} PluginInterfaceType;

typedef enum {
  PLUGIN_TYPE_UNKNOWN,
  PLUGIN_TYPE_UNSUPPORTED,
  PLUGIN_TYPE_EFFECT,
  PLUGIN_TYPE_INSTRUMENT,
  NUM_PLUGIN_TYPES
} PluginType;

typedef enum {
  PLUGIN_SETTING_TAIL_TIME_IN_MS,
  NUM_PLUGIN_SETTINGS
} PluginSetting;

typedef boolByte (*OpenPluginFunc)(void* pluginPtr);
typedef void (*PluginDisplayInfoFunc)(void* pluginPtr);
typedef void (*PluginGetAbsolutePathFunc)(void* pluginPtr, CharString outPath);
typedef int (*PluginGetSettingFunc)(void*, PluginSetting pluginSetting);
typedef void (*PluginProcessAudioFunc)(void* pluginPtr, SampleBuffer inputs, SampleBuffer outputs);
typedef void (*PluginProcessMidiEventsFunc)(void* pluginPtr, LinkedList midiEvents);
typedef void (*PluginSetParameterFunc)(void* pluginPtr, int index, float value);
typedef void (*PluginPrepareForProcessingFunc)(void* pluginPtr);
typedef void (*ClosePluginFunc)(void* pluginPtr);
typedef void (*FreePluginDataFunc)(void* pluginDataPtr);

typedef struct {
  PluginInterfaceType interfaceType;
  PluginType pluginType;
  CharString pluginName;
  CharString pluginLocation;

  OpenPluginFunc open;
  PluginDisplayInfoFunc displayInfo;
  PluginGetAbsolutePathFunc getAbsolutePath;
  PluginGetSettingFunc getSetting;
  PluginProcessAudioFunc processAudio;
  PluginProcessMidiEventsFunc processMidiEvents;
  PluginSetParameterFunc setParameter;
  PluginPrepareForProcessingFunc prepareForProcessing;
  ClosePluginFunc closePlugin;
  FreePluginDataFunc freePluginData;

  void* extraData;
} PluginMembers;

typedef PluginMembers* Plugin;

PluginInterfaceType guessPluginInterfaceType(const CharString pluginName, const CharString pluginRoot, CharString outLocation);
void listAvailablePlugins(const CharString pluginRoot);
void _logPluginLocation(const CharString location, PluginInterfaceType interfaceType);
Plugin newPlugin(PluginInterfaceType pluginInterfaceType, const CharString pluginName, const CharString pluginLocation);
void freePlugin(Plugin plugin);

#endif
