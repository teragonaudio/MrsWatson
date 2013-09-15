#include "PluginMock.h"


static void _pluginMockEmpty(void* pluginPtr) {
  // Nothing to do here
}

static boolByte _pluginMockOpen(void* pluginPtr) {
  Plugin self = (Plugin)pluginPtr;
  PluginMockData extraData = (PluginMockData)self->extraData;
  extraData->isOpen = true;
  return true;
}

static void _pluginMockGetAbsolutePath(void* pluginPtr, CharString outPath) {
  // Internal plugins don't have a path, and thus can't be copied. So just copy
  // an empty string here and let any callers needing the absolute path to check
  // for this value before doing anything important.
  charStringClear(outPath);
}

static int _pluginMockGetSetting(void* pluginPtr, PluginSetting pluginSetting) {
  switch(pluginSetting) {
    case PLUGIN_SETTING_TAIL_TIME_IN_MS:
      return kPluginMockTailTime;
    case PLUGIN_NUM_INPUTS:
      return 2;
    case PLUGIN_NUM_OUTPUTS:
      return 2;
    default:
      return 0;
  }
}

static void _pluginMockPrepareForProcessing(void* pluginPtr) {
  Plugin self = (Plugin)pluginPtr;
  PluginMockData extraData = (PluginMockData)self->extraData;
  extraData->isPrepared = true;
}

static void _pluginMockProcessAudio(void* pluginPtr, SampleBuffer inputs, SampleBuffer outputs) {
  Plugin self = (Plugin)pluginPtr;
  PluginMockData extraData = (PluginMockData)self->extraData;
  extraData->processAudioCalled = true;
  sampleBufferClear(outputs);
}

static void _pluginMockProcessMidiEvents(void* pluginPtr, LinkedList midiEvents) {
  Plugin self = (Plugin)pluginPtr;
  PluginMockData extraData = (PluginMockData)self->extraData;
  extraData->processMidiCalled = true;
}

static boolByte _pluginMockSetParameter(void* pluginPtr, unsigned int i, float value) {
  return false;
}

static void _pluginMockClose(void* pluginPtr) {
  Plugin self = (Plugin)pluginPtr;
  PluginMockData extraData = (PluginMockData)self->extraData;
  extraData->isOpen = false;
}

Plugin newPluginMock(void) {
  Plugin plugin = (Plugin)malloc(sizeof(PluginMembers));
  PluginMockData extraData = (PluginMockData)malloc(sizeof(PluginMockDataMembers));

  plugin->interfaceType = PLUGIN_TYPE_INTERNAL;
  plugin->pluginType = PLUGIN_TYPE_INSTRUMENT;
  plugin->pluginName = newCharStringWithCString("Mock");
  plugin->pluginLocation = newCharString();
  charStringCopyCString(plugin->pluginLocation, "Internal");

  plugin->openPlugin = _pluginMockOpen;
  plugin->displayInfo = _pluginMockEmpty;
  plugin->getAbsolutePath = _pluginMockGetAbsolutePath;
  plugin->getSetting = _pluginMockGetSetting;
  plugin->prepareForProcessing = _pluginMockPrepareForProcessing;
  plugin->processAudio = _pluginMockProcessAudio;
  plugin->processMidiEvents = _pluginMockProcessMidiEvents;
  plugin->setParameter = _pluginMockSetParameter;
  plugin->closePlugin = _pluginMockClose;
  plugin->freePluginData = _pluginMockEmpty;

  extraData->isOpen = false;
  extraData->isPrepared = false;
  extraData->processAudioCalled = false;
  extraData->processMidiCalled = false;
  plugin->extraData = extraData;

  return plugin;
}
