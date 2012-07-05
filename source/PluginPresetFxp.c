//
// PluginPresetFxp.c - MrsWatson
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
#include "PluginPresetFxp.h"
#include "EventLogger.h"
#include "PlatformUtilities.h"
#include "PluginVst2x.h"

static boolByte _openPluginPresetFxp(void* pluginPresetPtr) {
  PluginPreset pluginPreset = (PluginPreset)pluginPresetPtr;
  PluginPresetFxpData extraData = (PluginPresetFxpData)(pluginPreset->extraData);
  extraData->fileHandle = fopen(pluginPreset->presetName->data, "rb");
  if(extraData->fileHandle == NULL) {
    logError("Preset '%s' could not be opened for reading", pluginPreset->presetName->data);
    return false;
  }
  return true;
}

static boolByte _loadPluginPresetFxp(void* pluginPresetPtr, Plugin plugin) {
  PluginPreset pluginPreset = (PluginPreset)pluginPresetPtr;
  PluginPresetFxpData extraData = (PluginPresetFxpData)(pluginPreset->extraData);
  fxpProgram inProgram = (fxpProgram)malloc(sizeof(fxpProgramMembers));
  PluginPresetFxpProgramType programType;

  char* chunk;
  unsigned int chunkSize;
  unsigned int valueBuffer;
  unsigned int numObjectsRead;
  float parameterValue;
  unsigned int i;

  numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
  if(numObjectsRead != 1) {
    logError("Short read of FXP preset file at chunkMagic");
    return false;
  }
  inProgram->chunkMagic = convertBigEndianIntToPlatform(valueBuffer);
  if(inProgram->chunkMagic != 0x43636E4B) { // 'CcnK'
    logError("FXP preset file has bad chunk magic");
    return false;
  }

  numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
  if(numObjectsRead != 1) {
    logError("Short read of FXP preset file at byteSize");
    return false;
  }
  inProgram->byteSize = convertBigEndianIntToPlatform(valueBuffer);
  logDebug("FXP program has %d bytes in main chunk", inProgram->byteSize);

  numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
  if(numObjectsRead != 1) {
    logError("Short read of FXP preset file at fxMagic");
    return false;
  }
  inProgram->fxMagic = convertBigEndianIntToPlatform(valueBuffer);
  if(inProgram->fxMagic == 0x4678436b) { // 'FxCk'
    programType = FXP_TYPE_REGULAR;
  }
  else if(inProgram->fxMagic == 0x46504368) { // 'FPCh'
    programType = FXP_TYPE_OPAQUE_CHUNK;
  }
  else {
    logError("FXP preset has invalid fxMagic type");
    return false;
  }

  numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
  if(numObjectsRead != 1) {
    logError("Short read of FXP preset file at version");
    return false;
  }
  inProgram->version = convertBigEndianIntToPlatform(valueBuffer);

  // TODO: Need to check to make sure this matches the ID of the plugin
  numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
  if(numObjectsRead != 1) {
    logError("Short read of FXP preset file at fxID");
    return false;
  }
  inProgram->fxID = convertBigEndianIntToPlatform(valueBuffer);

  // TODO: Need to check to make sure this matches the version of the plugin
  numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
  if(numObjectsRead != 1) {
    logError("Short read of FXP preset file at fxVersion");
    return false;
  }
  inProgram->fxVersion = convertBigEndianIntToPlatform(valueBuffer);

  numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
  if(numObjectsRead != 1) {
    logError("Short read of FXP preset file at numParams");
    return false;
  }
  inProgram->numParams = convertBigEndianIntToPlatform(valueBuffer);

  memset(inProgram->prgName, 0, sizeof(char) * 28);
  numObjectsRead = fread(inProgram->prgName, sizeof(char), 28, extraData->fileHandle);
  if(numObjectsRead != 28) {
    logError("Short read of FXP preset file at prgName");
    return false;
  }
  copyToCharString(pluginPreset->presetName, inProgram->prgName);

  if(programType == FXP_TYPE_REGULAR) {
    for(i = 0; i < inProgram->numParams; i++) {
      float parameterBuffer = 0.0f;
      numObjectsRead = fread(&parameterBuffer, sizeof(float), 1, extraData->fileHandle);
      if(numObjectsRead != 1) {
        logError("Short read of FXP preset at parameter data");
        return false;
      }
      parameterValue = convertBigEndianFloatToPlatform(parameterBuffer);
      plugin->setParameter(plugin, i, parameterValue);
    }
  }
  else if(programType == FXP_TYPE_OPAQUE_CHUNK) {
    numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
    if(numObjectsRead != 1) {
      logError("Short read of FXP preset file at chunk size");
      return false;
    }
    inProgram->content.data.size = convertBigEndianIntToPlatform(valueBuffer);
    chunkSize = inProgram->content.data.size;
    if(chunkSize == 0) {
      logError("FXP preset has chunk of 0 bytes");
      return false;
    }

    chunk = (char*)malloc(sizeof(char) * chunkSize);
    memset(chunk, 0, sizeof(char) * chunkSize);
    numObjectsRead = fread(chunk, sizeof(char), chunkSize, extraData->fileHandle);
    if(numObjectsRead != chunkSize) {
      logError("Short read of FXP preset file at chunk");
      return false;
    }

    // The chunk has been read, set it to the actual plugin
    if(plugin->interfaceType == PLUGIN_TYPE_VST_2X) {
      setVst2xPluginChunk(plugin, chunk);
      free(chunk);
      return true;
    }
    else {
      logInternalError("Load FXP preset to wrong plugin type");
      return false;
    }
  }
  else {
    logInternalError("Invalid FXP program type");
    return false;
  }

  free(inProgram);
  return true;
}

static void _freePluginPresetDataFxp(void* extraDataPtr) {
  PluginPresetFxpData extraData = extraDataPtr;
  if(extraData->fileHandle != NULL) {
    fclose(extraData->fileHandle);
  }
  free(extraData);
}

PluginPreset newPluginPresetFxp(const CharString presetName) {
  PluginPreset pluginPreset = (PluginPreset)malloc(sizeof(PluginPresetMembers));
  PluginPresetFxpData extraData = (PluginPresetFxpData)malloc(sizeof(PluginPresetFxpDataMembers));

  pluginPreset->presetType = PRESET_TYPE_FXP;
  pluginPreset->presetName = newCharString();
  copyCharStrings(pluginPreset->presetName, presetName);
  pluginPreset->compatiblePluginTypes = 0;
  _setPresetCompatibleWithPluginType(pluginPreset, PLUGIN_TYPE_VST_2X);

  pluginPreset->openPreset = _openPluginPresetFxp;
  pluginPreset->loadPreset = _loadPluginPresetFxp;
  pluginPreset->freePresetData = _freePluginPresetDataFxp;

  pluginPreset->extraData = extraData;
  return pluginPreset;
}

