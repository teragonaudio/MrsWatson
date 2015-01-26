//
// PluginPresetOzone.c - MrsWatson
// Created by Nik Reiman on 24 Jan 15.
// Copyright (c) 2015 Teragon Audio. All rights reserved.
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

#include <string.h>
#include <base/Base64.h>
#include <base/File.h>
#include <base/CharString.h>
#include "PluginPresetOzone.h"
#include "base/File.h"
#include "plugin/PluginPreset.h"
#include "logging/EventLogger.h"

#include "PluginPreset.h"
#include "PluginVst2x.h"

static boolByte _openPluginPresetOzone(void *pluginPresetPtr)
{
    PluginPreset self = (PluginPreset)pluginPresetPtr;
    File presetFile = newFileWithPath(self->presetName);
    if (fileExists(presetFile)) {
        logDebug("Found Ozone preset file at '%s'", presetFile->absolutePath->data);
        PluginPresetOzoneData extraData = (PluginPresetOzoneData)self->extraData;
        extraData->presetFile = presetFile;
        return true;
    } else {
        freeFile(presetFile);
        return false;
    }
}

static boolByte _loadPluginPresetOzone(void *pluginPresetPtr, Plugin plugin)
{
    PluginPreset self = (PluginPreset)pluginPresetPtr;
    PluginPresetOzoneData extraData = (PluginPresetOzoneData)self->extraData;

    if (extraData->presetFile == NULL) {
        logError("No valid filehandle for Ozone preset '%s'", self->presetName->data);
        return false;
    }

    const size_t presetFileSize = fileGetSize(extraData->presetFile);
    if (presetFileSize == 0) {
        logError("Ozone preset file '%s' was empty", self->presetName->data);
        return false;
    }

    char *presetFileContents = fileReadBytes(extraData->presetFile, presetFileSize);
    const char *dspStateStart = "<DSPState>";
    const char *dspStateStop = "</DSPState>";
    const char *startPosition = strstr(presetFileContents, dspStateStart);
    const char *stopPosition = strstr(presetFileContents, dspStateStop);
    const char *firstDataByte = startPosition + strlen(dspStateStart);
    const size_t dspStateSize = stopPosition - firstDataByte;
    if (dspStateSize <= 0) {
        logError("Ozone preset '%s' has invalid chunk length %d",
                 self->presetName->data, dspStateSize);
        free(presetFileContents);
        return false;
    } else {
        logDebug("Read Ozone preset file, chunk size is %ld", dspStateSize);
    }

    extraData->chunk = base64Decode(firstDataByte, dspStateSize, &extraData->chunkSize);
    logDebug("Decoded %d bytes to binary chunk", extraData->chunkSize);
    free(presetFileContents);

    pluginVst2xSetProgramChunk(plugin, (char *)extraData->chunk, extraData->chunkSize);
    return true;
}

static void _freePluginPresetOzone(void *extraDataPtr)
{
    PluginPresetOzoneData extraData = (PluginPresetOzoneData)extraDataPtr;
    if (extraData->chunk != NULL) {
        free(extraData->chunk);
    }
    if (extraData->presetFile != NULL) {
        freeFile(extraData->presetFile);
    }
}

PluginPreset newPluginPresetOzone(const CharString presetName)
{
    PluginPreset self = (PluginPreset)malloc(sizeof(PluginPresetMembers));
    PluginPresetOzoneData extraData = (PluginPresetOzoneData)malloc(sizeof(PluginPresetOzoneDataMembers));

    self->presetType = PRESET_TYPE_OZONE;
    self->presetName = newCharString();
    charStringCopy(self->presetName, presetName);
    self->compatiblePluginTypes = 0;
    pluginPresetSetCompatibleWith(self, PLUGIN_TYPE_VST_2X);

    self->openPreset = _openPluginPresetOzone;
    self->loadPreset = _loadPluginPresetOzone;
    self->freePresetData = _freePluginPresetOzone;

    extraData->chunk = NULL;
    extraData->chunkSize = 0;
    self->extraData = extraData;

    return self;
}
