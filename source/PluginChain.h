//
//  PluginChain.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "Plugin.h"
#include "LinkedList.h"

#ifndef MrsWatson_PluginChain_h
#define MrsWatson_PluginChain_h

#define MAX_PLUGINS 8

typedef struct {
  int numPlugins;
  Plugin* plugins;
} PluginChainMembers;

typedef PluginChainMembers* PluginChain;

PluginChain newPluginChain(void);

boolean addPluginsFromArgumentString(PluginChain pluginChain, const CharString argumentString);
boolean initializePluginChain(PluginChain pluginChain);
void displayPluginInfo(PluginChain pluginChain);

void processPluginChainAudio(PluginChain pluginChain, SampleBuffer inBuffer, SampleBuffer outBuffer);
void processPluginChainMidiEvents(PluginChain pluginChain, LinkedList midiEvents);

void freePluginChain(PluginChain pluginChain);

#endif
