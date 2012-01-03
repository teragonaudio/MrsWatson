//
//  PluginChain.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#import <stdlib.h>
#import "PluginChain.h"

PluginChain newPluginChain(void) {
  PluginChain pluginChain = malloc(sizeof(PluginChainMembers));

  pluginChain->numPlugins = 0;
  pluginChain->plugins = malloc(sizeof(Plugin) * MAX_PLUGINS);

  return pluginChain;
}

void addPluginToChain(PluginChain pluginChain, Plugin plugin) {

}

void process(PluginChain pluginChain, SampleBuffer inBuffer, SampleBuffer outBuffer) {
  
}

void freePluginChain(PluginChain pluginChain) {

}
