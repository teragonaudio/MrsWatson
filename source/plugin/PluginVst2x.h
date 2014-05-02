//
// PluginVst2x.h - MrsWatson
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

#ifndef MrsWatson_PluginVst2x_h
#define MrsWatson_PluginVst2x_h

#include "base/CharString.h"
#include "plugin/Plugin.h"

static const char kPluginVst2xSubpluginSeparator = ':';

/**
 * List all available VST2.x plugins in common system locations. Note that this
 * function does not do recursive searches (yet).
 * @param pluginRoot User-provided plugin root path to search
 */
void listAvailablePluginsVst2x(const CharString pluginRoot);

/**
 * Create a new instance of a VST 2.x plugin
 * @param pluginName Plugin name
 * @param pluginRoot User-defined plugin root path
 * @return Initialized Plugin object, or NULL if no such plugin was found
 */
Plugin newPluginVst2x(const CharString pluginName, const CharString pluginRoot);

/**
 * Get the VST2.x unique ID
 * @param self
 * @return Unique ID, or 0 if not known (yes, this can happen)
 */
unsigned long pluginVst2xGetUniqueId(const Plugin self);

/**
 * Get the plugin's version number. Used to determine compatible FXB/FXP patches.
 * @param self
 * @return Plugin version, or 0 if an error occurred. Note that some (buggy) plugins
 *         could potentially have a declared version of 0 as well.
 */
unsigned long pluginVst2xGetVersion(const Plugin self);

/**
 * See if a VST2.x plugin exists with the given name. Absolute paths will also
 * be respected if passed.
 * @param pluginName Plugin name (short name or absolute path)
 * @param pluginRoot User-provided plugin root path
 * @return True if such a plugin exists in any location, false otherwise
 */
boolByte pluginVst2xExists(const CharString pluginName, const CharString pluginRoot);

/**
 * Set an internal program number for a VST2.x plugin.
 * @param self
 * @param programNumber Program to set
 * @return True if the program could be set and verified
 */
boolByte pluginVst2xSetProgram(Plugin self, const int programNumber);

/**
 * Set chuck preset data from an FXP preset to a VST2.x plugin.
 * @param self
 * @param chunk Chunk data to set
 * @param chunkSize Chunk size
 */
void pluginVst2xSetProgramChunk(Plugin self, char* chunk, size_t chunkSize);

#endif
