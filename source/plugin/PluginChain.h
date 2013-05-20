//
// PluginChain.h - MrsWatson
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

#ifndef MrsWatson_PluginChain_h
#define MrsWatson_PluginChain_h

#include "app/ReturnCodes.h"
#include "base/LinkedList.h"
#include "plugin/Plugin.h"
#include "plugin/PluginPreset.h"
#include "time/TaskTimer.h"

#define MAX_PLUGINS 8
#define CHAIN_STRING_PLUGIN_SEPARATOR ';'
#define CHAIN_STRING_PROGRAM_SEPARATOR ','

typedef struct {
  int numPlugins;
  Plugin* plugins;
  PluginPreset* presets;
} PluginChainMembers;
typedef PluginChainMembers* PluginChain;

PluginChain newPluginChain(void);

boolByte pluginChainAppend(PluginChain self, Plugin plugin, PluginPreset preset);
boolByte addPluginsFromArgumentString(PluginChain pluginChain, const CharString argumentString, const CharString userSearchPath);
ReturnCodes initializePluginChain(PluginChain pluginChain);

void displayPluginInfo(PluginChain pluginChain);
int getMaximumTailTimeInMs(PluginChain pluginChain);

void prepareForProcessing(PluginChain self);
void processPluginChainAudio(PluginChain pluginChain, SampleBuffer inBuffer, SampleBuffer outBuffer, TaskTimer taskTimer);
void processPluginChainMidiEvents(PluginChain pluginChain, LinkedList midiEvents, TaskTimer taskTimer);

void closePluginChain(PluginChain pluginChain);
void freePluginChain(PluginChain pluginChain);

#endif
