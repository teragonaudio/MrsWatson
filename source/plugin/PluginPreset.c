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

#include "base/File.h"
#include "logging/EventLogger.h"
#include "plugin/PluginPreset.h"
#include "plugin/PluginPresetFxp.h"
#include "plugin/PluginPresetInternalProgram.h"

static PluginPresetType _pluginPresetGuessType(const CharString presetName) {
  if (presetName == NULL || charStringIsEmpty(presetName)) {
    return PRESET_TYPE_INVALID;
  }

  File presetFile = newFileWithPath(presetName);
  CharString fileExtension = fileGetExtension(presetFile);
  freeFile(presetFile);

  if (fileExtension == NULL) {
    for (size_t i = 0; i < strlen(presetName->data); i++) {
      if (!charStringIsNumber(presetName, i)) {
        return PRESET_TYPE_INVALID;
      }
    }

    // If the preset name is all numeric, then it's an internal program number
    return PRESET_TYPE_INTERNAL_PROGRAM;
  } else if (charStringIsEqualToCString(fileExtension, "fxp", true)) {
    freeCharString(fileExtension);
    return PRESET_TYPE_FXP;
  } else {
    logCritical("Preset '%s' does not match any supported type",
                presetName->data);
    freeCharString(fileExtension);
    return PRESET_TYPE_INVALID;
  }
}

PluginPreset pluginPresetFactory(const CharString presetName) {
  PluginPresetType presetType = _pluginPresetGuessType(presetName);

  switch (presetType) {
  case PRESET_TYPE_FXP:
    return newPluginPresetFxp(presetName);

  case PRESET_TYPE_INTERNAL_PROGRAM:
    return newPluginPresetInternalProgram(presetName);

  default:
    return NULL;
  }
}

void pluginPresetSetCompatibleWith(PluginPreset pluginPreset,
                                   PluginInterfaceType interfaceType) {
  pluginPreset->compatiblePluginTypes |= (1 << interfaceType);
}

boolByte pluginPresetIsCompatibleWith(const PluginPreset pluginPreset,
                                      const Plugin plugin) {
  return (pluginPreset->compatiblePluginTypes & (1 << plugin->interfaceType));
}

void freePluginPreset(PluginPreset pluginPreset) {
  if (pluginPreset != NULL) {
    if (pluginPreset->extraData != NULL) {
      pluginPreset->freePresetData(pluginPreset->extraData);
      free(pluginPreset->extraData);
    }

    freeCharString(pluginPreset->presetName);
    free(pluginPreset);
  }
}
