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
    unsigned int numPlugins;
    Plugin *plugins;
    PluginPreset *presets;
    TaskTimer *audioTimers;
    TaskTimer *midiTimers;

    // Private fields
    boolByte _realtime;
    TaskTimer _realtimeTimer;
} PluginChainMembers;

/**
 * Class which holds multiple plugins which process audio in serial. Only one
 * instrument may be present in a plugin chain.
 */
typedef PluginChainMembers *PluginChain;

/**
 * Get a reference to the global plugin chain instance.
 * @return Reference to global plugin chain, or NULL if the global instance has
 * not yet been initialized.
 */
PluginChain getPluginChain(void);

/**
 * Initialize the global plugin chain instance. Should be called fairly
 * early in the program initialization.
 */
void initPluginChain(void);

/**
 * Append a plugin to the end of the chain
 * @param self
 * @param plugin Plugin to add
 * @param preset Preset to be loaded into the plugin. If no preset is desired,
 * passed NULL here.
 * @return True if the plugin could be added to the end of the chain
 */
boolByte pluginChainAppend(PluginChain self, Plugin plugin, PluginPreset preset);

// TODO: Deprecate and remove this function
boolByte pluginChainAddFromArgumentString(PluginChain self, const CharString argumentString, const CharString userSearchPath);

/**
 * Open and initialize all plugins in the chain.
 * @param self
 * @return RETURN_CODE_SUCCESS on success, other code on failure
 */
ReturnCode pluginChainInitialize(PluginChain self);

/**
 * Inspect each plugin in the chain
 * @param self
 */
void pluginChainInspect(PluginChain self);

/**
 * Get the maximum amount of tail time (post*processing time with empty input)
 * needed for the chain. This is essentially the largest tail time value for any
 * plug-in in the chain.
 * @param self
 * @return Maximum tail time, in milliseconds
 */
int pluginChainGetMaximumTailTimeInMs(PluginChain self);

/**
 * Get the total processing delay in frames.
 * @param self
 * @return Total processing delay, in frames.
 */
unsigned long pluginChainGetProcessingDelay(PluginChain self);

/**
 * Set parameters on the first plugin in a chain.
 * @param self
 * @param parameters List of parameters to be applied
 * @return True if all parameters were set, false otherwise
 */
boolByte pluginChainSetParameters(PluginChain self, const LinkedList parameters);

/**
 * Set realtime mode for the plugin chain. When set, calls to pluginChainProcessAudio()
 * will sleep for the additional time required to process the block in realtime.
 * @param realtime True to enable realtime mode, false to disable (default)
 * @param self
 */
void pluginChainSetRealtime(PluginChain self, boolByte realtime);

/**
 * Prepare each plugin in the chain for processing. This should be called before
 * the first block of audio is sent to the chain.
 * @param self
 */
void pluginChainPrepareForProcessing(PluginChain self);

/**
 * Process a single block of samples through each plugin in the chain.
 * @param self
 * @param inBuffer Input sample block
 * @param outBuffer Output sample block
 */
void pluginChainProcessAudio(PluginChain self, SampleBuffer inBuffer, SampleBuffer outBuffer);

/**
 * Send a list of MIDI events to be processed by the chain. Currently, only the
 * first plugin in the chain will receive these events.
 * @param self
 * @param midiEvents List of events to process
 */
void pluginChainProcessMidi(PluginChain self, LinkedList midiEvents);

/**
 * Close all plugins in the chain
 * @param self
 */
void pluginChainShutdown(PluginChain self);

/**
 * Free the plugin chain and all associated resources (including all plugins)
 * @param self
 */
void freePluginChain(PluginChain self);

#endif
