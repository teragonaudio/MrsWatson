//
// PluginPreset.c - MrsWatson
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PluginPreset.h"
#include "StringUtilities.h"
#include "EventLogger.h"

PluginPresetType guessPluginPresetType(const CharString presetName) {
  const char* fileExtension = getFileExtension(presetName->data);
  if(fileExtension == NULL) {
    return PRESET_TYPE_INVALID;
  }
  else if(!strcasecmp(fileExtension, "fxp")) {
    return PRESET_TYPE_FXP;
  }
  else {
    logCritical("Preset '%s' does not match any supported type", presetName->data);
    return PRESET_TYPE_INVALID;
  }
}

PluginPreset newPluginPreset(PluginPresetType presetType, const CharString presetName) {
  switch(presetType) {
    case PRESET_TYPE_FXP:
      return newPluginPresetFxp(presetName);
    default:
      return NULL;
  }
}

void _setPresetCompatibleWithPluginType(PluginPreset pluginPreset, PluginInterfaceType interfaceType) {
  pluginPreset->compatiblePluginTypes |= (1 << interfaceType);
}

boolean isPresetCompatibleWithPlugin(const PluginPreset pluginPreset, const Plugin plugin) {
  return pluginPreset & (1 << plugin->interfaceType);
}

boolean loadPreset(const PluginPreset pluginPreset, Plugin plugin) {
  return pluginPreset->loadPreset(pluginPreset, plugin);
}

void freePluginPreset(PluginPreset pluginPreset) {
  pluginPreset->freePresetData(pluginPreset->extraData);
  freeCharString(pluginPreset->presetName);
  free(pluginPreset);
}
