//
// PluginPresetMock.c - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
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

#include "PluginPresetMock.h"

static boolByte _openPluginPresetMock(void *pluginPresetPtr) {
  PluginPreset pluginPreset = (PluginPreset)pluginPresetPtr;
  PluginPresetMockData extraData =
      (PluginPresetMockData)pluginPreset->extraData;
  extraData->isOpen = true;
  return true;
}

static boolByte _loadPluginPresetMock(void *pluginPresetPtr, Plugin plugin) {
  PluginPreset pluginPreset = (PluginPreset)pluginPresetPtr;
  PluginPresetMockData extraData =
      (PluginPresetMockData)pluginPreset->extraData;
  extraData->isLoaded = true;
  return true;
}

static void _freePluginPresetMock(void *extraDataPtr) {}

PluginPreset newPluginPresetMock(void) {
  PluginPreset pluginPreset = (PluginPreset)malloc(sizeof(PluginPresetMembers));
  PluginPresetMockData extraData =
      (PluginPresetMockData)malloc(sizeof(PluginPresetMockDataMembers));

  pluginPreset->presetType = PRESET_TYPE_INTERNAL_PROGRAM;
  pluginPreset->presetName = newCharString();
  pluginPreset->compatiblePluginTypes = 0;
  pluginPresetSetCompatibleWith(pluginPreset, PLUGIN_TYPE_INTERNAL);

  pluginPreset->openPreset = _openPluginPresetMock;
  pluginPreset->loadPreset = _loadPluginPresetMock;
  pluginPreset->freePresetData = _freePluginPresetMock;

  extraData->isOpen = false;
  extraData->isLoaded = false;
  pluginPreset->extraData = extraData;

  return pluginPreset;
}
