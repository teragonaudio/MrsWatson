//
// PluginMock.c - MrsWatson
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

#include "PluginMock.h"

static void _pluginMockEmpty(void *pluginPtr) {
  // Nothing to do here
}

static boolByte _pluginMockOpen(void *pluginPtr) {
  Plugin self = (Plugin)pluginPtr;
  PluginMockData extraData = (PluginMockData)self->extraData;
  extraData->isOpen = true;
  return true;
}

static int _pluginMockGetSetting(void *pluginPtr, PluginSetting pluginSetting) {
  switch (pluginSetting) {
  case PLUGIN_SETTING_TAIL_TIME_IN_MS:
    return kPluginMockTailTime;

  case PLUGIN_NUM_INPUTS:
    return 2;

  case PLUGIN_NUM_OUTPUTS:
    return 2;

  case PLUGIN_INITIAL_DELAY:
    return 0;

  default:
    return 0;
  }
}

static void _pluginMockPrepareForProcessing(void *pluginPtr) {
  Plugin self = (Plugin)pluginPtr;
  PluginMockData extraData = (PluginMockData)self->extraData;
  extraData->isPrepared = true;
}

static void _pluginMockProcessAudio(void *pluginPtr, SampleBuffer inputs,
                                    SampleBuffer outputs) {
  Plugin self = (Plugin)pluginPtr;
  PluginMockData extraData = (PluginMockData)self->extraData;
  extraData->processAudioCalled = true;
  sampleBufferClear(outputs);
}

static void _pluginMockProcessMidiEvents(void *pluginPtr,
                                         LinkedList midiEvents) {
  Plugin self = (Plugin)pluginPtr;
  PluginMockData extraData = (PluginMockData)self->extraData;
  extraData->processMidiCalled = true;
}

static boolByte _pluginMockSetParameter(void *pluginPtr, unsigned int i,
                                        float value) {
  return false;
}

static void _pluginMockClose(void *pluginPtr) {
  Plugin self = (Plugin)pluginPtr;
  PluginMockData extraData = (PluginMockData)self->extraData;
  extraData->isOpen = false;
}

Plugin newPluginMock(void) {
  Plugin plugin = _newPlugin(PLUGIN_TYPE_INTERNAL, PLUGIN_TYPE_INSTRUMENT);
  charStringCopyCString(plugin->pluginName, "Mock");
  charStringCopyCString(plugin->pluginLocation, "Internal");

  plugin->openPlugin = _pluginMockOpen;
  plugin->displayInfo = _pluginMockEmpty;
  plugin->getSetting = _pluginMockGetSetting;
  plugin->prepareForProcessing = _pluginMockPrepareForProcessing;
  plugin->processAudio = _pluginMockProcessAudio;
  plugin->processMidiEvents = _pluginMockProcessMidiEvents;
  plugin->setParameter = _pluginMockSetParameter;
  plugin->closePlugin = _pluginMockClose;
  plugin->freePluginData = _pluginMockEmpty;

  PluginMockData extraData =
      (PluginMockData)malloc(sizeof(PluginMockDataMembers));
  extraData->isOpen = false;
  extraData->isPrepared = false;
  extraData->processAudioCalled = false;
  extraData->processMidiCalled = false;
  plugin->extraData = extraData;

  return plugin;
}
