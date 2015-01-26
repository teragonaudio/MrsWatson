//
// PluginPreset.h - MrsWatson
// Created by Nik Reiman on 1/13/12.
// Copyright (c) 2011 Teragon Audio. All rights reserved.
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

#ifndef MrsWatson_PluginPreset_h
#define MrsWatson_PluginPreset_h

#include "base/CharString.h"
#include "plugin/Plugin.h"

typedef enum {
    PRESET_TYPE_INVALID,
    PRESET_TYPE_FXP,
    PRESET_TYPE_INTERNAL_PROGRAM,
    PRESET_TYPE_OZONE,
    NUM_PRESET_TYPES
} PluginPresetType;

/**
 * Called when the preset is to be loaded from the filesystem
 * @param pluginPresetPtr self
 */
typedef boolByte (*OpenPresetFunc)(void *pluginPresetPtr);
/**
 * Called when the preset is to be loaded into a plugin
 * @param plugin Plugin which will receive the preset. This should check that
 * the preset is compatible with the given plugging before loading it.
 * @param pluginPresetPtr self
 * @return True on success, false on failure
 */
typedef boolByte (*LoadPresetFunc)(void *pluginPresetPtr, Plugin plugin);
/**
 * Free a preset and it's related data
 * @param pluginPresetPtr self
 */
typedef void (*FreePresetDataFunc)(void *pluginPresetPtr);

typedef struct {
    PluginPresetType presetType;
    CharString presetName;
    unsigned int compatiblePluginTypes;

    OpenPresetFunc openPreset;
    LoadPresetFunc loadPreset;
    FreePresetDataFunc freePresetData;

    void *extraData;
} PluginPresetMembers;

/**
 * Class which is used to hold preset data which will be loaded into a plugin
 * before audio processing.
 */
typedef PluginPresetMembers *PluginPreset;

/**
 * Create a new plugin preset from a given name. Usually this function inspects
 * the name and guesses an appropriate handler based on the file extension.
 * @param presetName Preset name
 * @return Initialized PluginPreset or NULL if no preset type found
 */
PluginPreset pluginPresetFactory(const CharString presetName);

/**
 * Check if a preset will be compatible with a plugin. Some plugin interface
 * types have extra safety checks to make sure that a preset must match the
 * plugin's ID, this call essentially wraps this functionality.
 * @param self
 * @param plugin Plugin to check against
 * @return True if the preset can be loaded into the plugin
 */
boolByte pluginPresetIsCompatibleWith(const PluginPreset self, const Plugin plugin);

/**
 * Set interface compatibility for a preset type. This function should only be
 * called from a PluginPreset subclass, you should not have to manually call
 * this in normal host operations.
 * @param self
 * @param interfaceType Interface type to set
 */
void pluginPresetSetCompatibleWith(PluginPreset self, PluginInterfaceType interfaceType);

/**
 * Free a PluginPreset and all associated resources
 * @param self
 */
void freePluginPreset(PluginPreset self);

#endif
