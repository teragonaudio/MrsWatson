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
