//
//  PluginChain.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#import "Plugin.h"

#ifndef MrsWatson_PluginChain_h
#define MrsWatson_PluginChain_h

#define MAX_PLUGINS 8

typedef struct {
  int numPlugins;
  Plugin* plugins;
} PluginChainMembers;

typedef PluginChainMembers* PluginChain;

PluginChain newPluginChain(void);
void addPluginsFromArgumentString(PluginChain pluginChain, const CharString argumentString);
void initializePluginChain(PluginChain pluginChain);
void process(PluginChain pluginChain, SampleBuffer inBuffer, SampleBuffer outBuffer);
void freePluginChain(PluginChain pluginChain);

#endif
