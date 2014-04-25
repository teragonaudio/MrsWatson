#include "unit/TestRunner.h"
#include "plugin/PluginChain.h"
#include "plugin/PluginPassthru.h"
#include "PluginMock.h"
#include "PluginPresetMock.h"

#include "audio/AudioSettings.h"
#include "midi/MidiEvent.h"

static int _testNewPluginChain(void) {
  PluginChain p = newPluginChain();
  assertIntEquals(p->numPlugins, 0);
  assertNotNull(p->plugins);
  assertNotNull(p->presets);
  freePluginChain(p);
  return 0;
}

static int _testAddFromArgumentStringNull(void) {
  PluginChain p = newPluginChain();
  CharString c = newCharStringWithCString("/");

  assertFalse(pluginChainAddFromArgumentString(p, NULL, c));
  assertIntEquals(p->numPlugins, 0);

  freePluginChain(p);
  freeCharString(c);
  return 0;
}

static int _testAddFromArgumentStringEmpty(void) {
  PluginChain p = newPluginChain();
  CharString c = newCharStringWithCString("/");
  CharString empty = newCharString();

  assertFalse(pluginChainAddFromArgumentString(p, empty, c));
  assertIntEquals(p->numPlugins, 0);

  freePluginChain(p);
  freeCharString(c);
  freeCharString(empty);
  return 0;
}

static int _testAddFromArgumentStringEmptyLocation(void) {
  PluginChain p = newPluginChain();
  CharString c = newCharStringWithCString(kInternalPluginPassthruName);
  CharString empty = newCharString();

  assert(pluginChainAddFromArgumentString(p, c, empty));
  assertIntEquals(p->numPlugins, 1);

  freePluginChain(p);
  freeCharString(c);
  freeCharString(empty);
  return 0;
}

static int _testAddFromArgumentStringNullLocation(void) {
  PluginChain p = newPluginChain();
  CharString c = newCharStringWithCString(kInternalPluginPassthruName);

  assert(pluginChainAddFromArgumentString(p, c, NULL));
  assertIntEquals(p->numPlugins, 1);

  freePluginChain(p);
  freeCharString(c);
  return 0;
}

static int _testAddFromArgumentString(void) {
  PluginChain p = newPluginChain();
  CharString testArgs = newCharStringWithCString(kInternalPluginPassthruName);

  assert(pluginChainAddFromArgumentString(p, testArgs, NULL));
  assertIntEquals(p->numPlugins, 1);
  assertNotNull(p->plugins[0]);
  assertIntEquals(p->plugins[0]->pluginType, PLUGIN_TYPE_INTERNAL);
  assertCharStringEquals(p->plugins[0]->pluginName, kInternalPluginPassthruName);

  freePluginChain(p);
  freeCharString(testArgs);
  return 0;
}

static int _testAddFromArgumentStringMultiple(void) {
  PluginChain p = newPluginChain();
  CharString testArgs = newCharStringWithCString(kInternalPluginPassthruName);
  unsigned int i;

  assert(pluginChainAddFromArgumentString(p, testArgs, NULL));
  assert(pluginChainAddFromArgumentString(p, testArgs, NULL));
  assertIntEquals(p->numPlugins, 2);
  for(i = 0; i < p->numPlugins; i++) {
    assertNotNull(p->plugins[i]);
    assertIntEquals(p->plugins[i]->pluginType, PLUGIN_TYPE_INTERNAL);
    assertCharStringEquals(p->plugins[i]->pluginName, kInternalPluginPassthruName);
  }

  freePluginChain(p);
  freeCharString(testArgs);
  return 0;
}

static int _testAddPluginWithPresetFromArgumentString(void) {
  PluginChain p = newPluginChain();
  CharString testArgs = newCharStringWithCString("mrs_passthru,testPreset.fxp");

  assert(pluginChainAddFromArgumentString(p, testArgs, NULL));
  assertIntEquals(p->numPlugins, 1);
  assertIntEquals(p->plugins[0]->pluginType, PLUGIN_TYPE_INTERNAL);
  assertCharStringEquals(p->plugins[0]->pluginName, kInternalPluginPassthruName);
  assertNotNull(p->presets[0]);
  assertCharStringEquals(p->presets[0]->presetName, "testPreset.fxp");

  freePluginChain(p);
  freeCharString(testArgs);
  return 0;
}

static int _testAddFromArgumentStringWithPresetSpaces(void) {
  PluginChain p = newPluginChain();
  CharString testArgs = newCharStringWithCString("mrs_passthru,test preset.fxp");

  assert(pluginChainAddFromArgumentString(p, testArgs, NULL));
  assertIntEquals(p->numPlugins, 1);
  assertIntEquals(p->plugins[0]->pluginType, PLUGIN_TYPE_INTERNAL);
  assertCharStringEquals(p->plugins[0]->pluginName, kInternalPluginPassthruName);
  assertNotNull(p->presets[0]);
  assertCharStringEquals(p->presets[0]->presetName, "test preset.fxp");

  freePluginChain(p);
  freeCharString(testArgs);
  return 0;
}

static int _testAppendPlugin(void) {
  Plugin mock = newPluginMock();
  PluginChain p = newPluginChain();

  assert(pluginChainAppend(p, mock, NULL));

  freePluginChain(p);
  return 0;
}

static int _testAppendWithNullPlugin(void) {
  PluginChain p = newPluginChain();

  assertFalse(pluginChainAppend(p, NULL, NULL));

  freePluginChain(p);
  return 0;  
}

static int _testAppendWithPreset(void) {
  Plugin mock = newPluginMock();
  PluginChain p = newPluginChain();
  PluginPreset mockPreset = newPluginPresetMock();

  assert(pluginChainAppend(p, mock, mockPreset));

  freePluginChain(p);
  return 0;  
}

static int _testInitializePluginChain(void) {
  Plugin mock = newPluginMock();
  PluginChain p = newPluginChain();
  PluginPreset mockPreset = newPluginPresetMock();

  assert(pluginChainAppend(p, mock, mockPreset));
  assert(((PluginMockData)mock->extraData)->isOpen);
  assert(((PluginPresetMockData)mockPreset->extraData)->isOpen);
  assert(((PluginPresetMockData)mockPreset->extraData)->isLoaded);

  freePluginChain(p);
  return 0;
}

static int _testGetMaximumTailTime(void) {
  Plugin mock = newPluginMock();
  PluginChain p = newPluginChain();
  int maxTailTime = 0;

  assert(pluginChainAppend(p, mock, NULL));
  maxTailTime = pluginChainGetMaximumTailTimeInMs(p);
  assertIntEquals(maxTailTime, kPluginMockTailTime);

  freePluginChain(p);
  return 0;
}

static int _testPrepareForProcessing(void) {
  Plugin mock = newPluginMock();
  PluginChain p = newPluginChain();

  assert(pluginChainAppend(p, mock, NULL));
  pluginChainPrepareForProcessing(p);
  assert(((PluginMockData)mock->extraData)->isPrepared);

  freePluginChain(p);
  return 0;  
}

static int _testProcessPluginChainAudio(void) {
  Plugin mock = newPluginMock();
  PluginChain p = newPluginChain();
  SampleBuffer inBuffer = newSampleBuffer(DEFAULT_NUM_CHANNELS, DEFAULT_BLOCKSIZE);
  SampleBuffer outBuffer = newSampleBuffer(DEFAULT_NUM_CHANNELS, DEFAULT_BLOCKSIZE);

  assert(pluginChainAppend(p, mock, NULL));
  pluginChainProcessAudio(p, inBuffer, outBuffer);
  assert(((PluginMockData)mock->extraData)->processAudioCalled);

  freePluginChain(p);
  freeSampleBuffer(inBuffer);
  freeSampleBuffer(outBuffer);
  return 0;
}

static int _testProcessPluginChainMidiEvents(void) {
  Plugin mock = newPluginMock();
  PluginChain p = newPluginChain();
  SampleBuffer inBuffer = newSampleBuffer(DEFAULT_NUM_CHANNELS, DEFAULT_BLOCKSIZE);
  SampleBuffer outBuffer = newSampleBuffer(DEFAULT_NUM_CHANNELS, DEFAULT_BLOCKSIZE);
  LinkedList list = newLinkedList();
  MidiEvent midi = newMidiEvent();

  linkedListAppend(list, midi);
  assert(pluginChainAppend(p, mock, NULL));
  pluginChainProcessMidi(p, list);
  assert(((PluginMockData)mock->extraData)->processMidiCalled);

  freeMidiEvent(midi);
  freeLinkedList(list);
  freePluginChain(p);
  freeSampleBuffer(inBuffer);
  freeSampleBuffer(outBuffer);
  return 0;
}

static int _testShutdown(void) {
  Plugin mock = newPluginMock();
  PluginChain p = newPluginChain();

  assert(pluginChainAppend(p, mock, NULL));
  pluginChainShutdown(p);
  assertFalse(((PluginMockData)mock->extraData)->isOpen);

  freePluginChain(p);
  return 0;
}

TestSuite addPluginChainTests(void);
TestSuite addPluginChainTests(void) {
  TestSuite testSuite = newTestSuite("PluginChain", NULL, NULL);
  addTest(testSuite, "NewObject", _testNewPluginChain);

  addTest(testSuite, "AddFromArgumentStringNull", _testAddFromArgumentStringNull);
  addTest(testSuite, "AddFromArgumentStringEmpty", _testAddFromArgumentStringEmpty);
  addTest(testSuite, "AddFromArgumentStringEmptyLocation", _testAddFromArgumentStringEmptyLocation);
  addTest(testSuite, "AddFromArgumentStringNullLocation", _testAddFromArgumentStringNullLocation);
  addTest(testSuite, "AddFromArgumentString", _testAddFromArgumentString);
  addTest(testSuite, "AddFromArgumentStringMultiple", _testAddFromArgumentStringMultiple);
  addTest(testSuite, "AddPluginWithPresetFromArgumentString", _testAddPluginWithPresetFromArgumentString);
  addTest(testSuite, "AddFromArgumentStringWithPresetSpaces", _testAddFromArgumentStringWithPresetSpaces);
  addTest(testSuite, "AppendPlugin", _testAppendPlugin);
  addTest(testSuite, "AppendWithNullPlugin", _testAppendWithNullPlugin);
  addTest(testSuite, "AppendWithPreset", _testAppendWithPreset);
  addTest(testSuite, "InitializePluginChain", _testInitializePluginChain);

  addTest(testSuite, "GetMaximumTailTime", _testGetMaximumTailTime);

  addTest(testSuite, "PrepareForProcessing", _testPrepareForProcessing);
  addTest(testSuite, "ProcessPluginChainAudio", _testProcessPluginChainAudio);
  addTest(testSuite, "ProcessPluginChainMidiEvents", _testProcessPluginChainMidiEvents);

  addTest(testSuite, "Shutdown", _testShutdown);

  return testSuite;
}
