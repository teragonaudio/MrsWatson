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

static boolean _openPluginPresetFxp(void* pluginPresetPtr) {
  PluginPreset pluginPreset = pluginPresetPtr;
  PluginPresetFxpData extraData = pluginPreset->extraData;
  extraData->fileHandle = fopen(pluginPreset->presetName->data, "rb");
  if(extraData->fileHandle == NULL) {
    logError("Preset '%s' could not be opened for reading", pluginPreset->presetName->data);
    return false;
  }
  return true;
}

static boolean _loadPluginPresetFxp(void* pluginPresetPtr, Plugin plugin) {
  PluginPreset pluginPreset = pluginPresetPtr;
  PluginPresetFxpData extraData = pluginPreset->extraData;
  fxpProgram inProgram = malloc(sizeof(fxpProgramMembers));

  unsigned int valueBuffer;
  unsigned int numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
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
  }
  inProgram->byteSize = convertBigEndianIntToPlatform(valueBuffer);
  logDebug("FXP program has %d bytes in main chunk", inProgram->byteSize);

  PluginPresetFxpProgramType programType = FXP_TYPE_INVALID;
  numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
  if(numObjectsRead != 1) {
    logError("Short read of FXP preset file at fxMagic");
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
  }
  inProgram->version = convertIntToBigEndian(valueBuffer);

  numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
  if(numObjectsRead != 1) {
    logError("Short read of FXP preset file at fxID");
  }
  inProgram->fxID = convertIntToBigEndian(valueBuffer);

  numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
  if(numObjectsRead != 1) {
    logError("Short read of FXP preset file at fxVersion");
  }
  inProgram->fxVersion = convertIntToBigEndian(valueBuffer);

  numObjectsRead = fread(&valueBuffer, sizeof(unsigned int), 1, extraData->fileHandle);
  if(numObjectsRead != 1) {
    logError("Short read of FXP preset file at numParams");
  }
  inProgram->numParams = convertIntToBigEndian(valueBuffer);

  memset(inProgram->prgName, 0, sizeof(char) * 28);
  numObjectsRead = fread(inProgram->prgName, sizeof(char), 28, extraData->fileHandle);
  if(numObjectsRead != 28) {
    logError("Short read of FXP preset file at prgName");
  }
  copyToCharString(pluginPreset->presetName, inProgram->prgName);

  if(programType == FXP_TYPE_REGULAR) {
  }
  else if(programType == FXP_TYPE_OPAQUE_CHUNK) {
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
  PluginPreset pluginPreset = malloc(sizeof(PluginPresetMembers));
  pluginPreset->presetType = PRESET_TYPE_FXP;
  pluginPreset->presetName = newCharString();
  copyCharStrings(pluginPreset->presetName, presetName);
  pluginPreset->compatiblePluginTypes = 0;
  _setPresetCompatibleWithPluginType(pluginPreset, PLUGIN_TYPE_VST_2X);

  pluginPreset->openPreset = _openPluginPresetFxp;
  pluginPreset->loadPreset = _loadPluginPresetFxp;
  pluginPreset->freePresetData = _freePluginPresetDataFxp;

  PluginPresetFxpData extraData = malloc(sizeof(PluginPresetFxpDataMembers));
  pluginPreset->extraData = extraData;
  return pluginPreset;
}

