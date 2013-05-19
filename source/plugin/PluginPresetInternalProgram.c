//
// PluginPresetInternalProgram.c - MrsWatson
// Created by Nik Reiman on 19 May 13.
// Copyright (c) 2013 Teragon Audio. All rights reserved.
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

#include <stdlib.h>

#include "base/CharString.h"
#include "plugin/PluginPresetInternalProgram.h"
#include "plugin/PluginVst2x.h"

static boolByte _openPluginPresetInternalProgram(void* pluginPresetPtr) {
  PluginPreset pluginPreset = (PluginPreset)pluginPresetPtr;
  PluginPresetInternalProgramData extraData = (PluginPresetInternalProgramData)pluginPreset->extraData;
  extraData->programNumber = strtol(pluginPreset->presetName->data, NULL, 10);
  return true;
}

static boolByte _loadPluginPresetInternalProgram(void* pluginPresetPtr, Plugin plugin) {
  PluginPreset pluginPreset = (PluginPreset)pluginPresetPtr;
  PluginPresetInternalProgramData extraData = (PluginPresetInternalProgramData)pluginPreset->extraData;
  return setVst2xProgram(plugin, extraData->programNumber);
}

static void _freePluginPresetInternalProgram(void* extraDataPtr) {
  free(extraDataPtr);
}

PluginPreset newPluginPresetInternalProgram(const CharString presetName) {
  PluginPreset pluginPreset = (PluginPreset)malloc(sizeof(PluginPresetMembers));
  PluginPresetInternalProgramData extraData; // Yay for long type names!
  extraData = (PluginPresetInternalProgramData)malloc(sizeof(PluginPresetInternalProgramDataMembers));

  pluginPreset->presetType = PRESET_TYPE_INTERNAL_PROGRAM;
  pluginPreset->presetName = newCharString();
  charStringCopy(pluginPreset->presetName, presetName);
  pluginPreset->compatiblePluginTypes = 0;
  _setPresetCompatibleWithPluginType(pluginPreset, PLUGIN_TYPE_VST_2X);

  pluginPreset->openPreset = _openPluginPresetInternalProgram;
  pluginPreset->loadPreset = _loadPluginPresetInternalProgram;
  pluginPreset->freePresetData = _freePluginPresetInternalProgram;

  extraData->programNumber = 0;
  pluginPreset->extraData = extraData;
  
  return pluginPreset;
}
