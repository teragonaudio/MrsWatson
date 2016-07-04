#include "plugin/PluginChain.h"
#include "audio/AudioSettings.h"
#include "midi/MidiEvent.h"
#include "plugin/PluginPassthru.h"
#include "unit/TestRunner.h"

#include "PluginMock.h"
#include "PluginPresetMock.h"

static void _pluginChainTestSetup(void) { initPluginChain(); }

static void _pluginChainTestTeardown(void) {
  freePluginChain(getPluginChain());
}

static int _testInitPluginChain(void) {
  PluginChain p = getPluginChain();
  assertIntEquals(0, p->numPlugins);
  assertNotNull(p->plugins);
  assertNotNull(p->presets);
  return 0;
}

static int _testAddFromArgumentStringNull(void) {
  PluginChain p = getPluginChain();
  CharString c = newCharStringWithCString("/");

  assertFalse(pluginChainAddFromArgumentString(p, NULL, c));
  assertIntEquals(0, p->numPlugins);

  freeCharString(c);
  return 0;
}

static int _testAddFromArgumentStringEmpty(void) {
  PluginChain p = getPluginChain();
  CharString c = newCharStringWithCString("/");
  CharString empty = newCharString();

  assertFalse(pluginChainAddFromArgumentString(p, empty, c));
  assertIntEquals(0, p->numPlugins);

  freeCharString(c);
  freeCharString(empty);
  return 0;
}

static int _testAddFromArgumentStringEmptyLocation(void) {
  PluginChain p = getPluginChain();
  CharString c = newCharStringWithCString(kInternalPluginPassthruName);
  CharString empty = newCharString();

  assert(pluginChainAddFromArgumentString(p, c, empty));
  assertIntEquals(1, p->numPlugins);

  freeCharString(c);
  freeCharString(empty);
  return 0;
}

static int _testAddFromArgumentStringNullLocation(void) {
  PluginChain p = getPluginChain();
  CharString c = newCharStringWithCString(kInternalPluginPassthruName);

  assert(pluginChainAddFromArgumentString(p, c, NULL));
  assertIntEquals(1, p->numPlugins);

  freeCharString(c);
  return 0;
}

static int _testAddFromArgumentString(void) {
  PluginChain p = getPluginChain();
  CharString testArgs = newCharStringWithCString(kInternalPluginPassthruName);

  assert(pluginChainAddFromArgumentString(p, testArgs, NULL));
  assertIntEquals(1, p->numPlugins);
  assertNotNull(p->plugins[0]);
  assertIntEquals(PLUGIN_TYPE_INTERNAL, p->plugins[0]->pluginType);
  assertCharStringEquals(kInternalPluginPassthruName,
                         p->plugins[0]->pluginName);

  freeCharString(testArgs);
  return 0;
}

static int _testAddFromArgumentStringMultiple(void) {
  PluginChain p = getPluginChain();
  CharString testArgs = newCharStringWithCString(kInternalPluginPassthruName);
  unsigned int i;

  assert(pluginChainAddFromArgumentString(p, testArgs, NULL));
  assert(pluginChainAddFromArgumentString(p, testArgs, NULL));
  assertIntEquals(2, p->numPlugins);

  for (i = 0; i < p->numPlugins; i++) {
    assertNotNull(p->plugins[i]);
    assertIntEquals(PLUGIN_TYPE_INTERNAL, p->plugins[i]->pluginType);
    assertCharStringEquals(kInternalPluginPassthruName,
                           p->plugins[i]->pluginName);
  }

  freeCharString(testArgs);
  return 0;
}

static int _testAddPluginWithPresetFromArgumentString(void) {
  PluginChain p = getPluginChain();
  CharString testArgs = newCharStringWithCString("mrs_passthru,testPreset.fxp");

  assert(pluginChainAddFromArgumentString(p, testArgs, NULL));
  assertIntEquals(1, p->numPlugins);
  assertIntEquals(PLUGIN_TYPE_INTERNAL, p->plugins[0]->pluginType);
  assertCharStringEquals(kInternalPluginPassthruName,
                         p->plugins[0]->pluginName);
  assertNotNull(p->presets[0]);
  assertCharStringEquals("testPreset.fxp", p->presets[0]->presetName);

  freeCharString(testArgs);
  return 0;
}

static int _testAddFromArgumentStringWithPresetSpaces(void) {
  PluginChain p = getPluginChain();
  CharString testArgs =
      newCharStringWithCString("mrs_passthru,test preset.fxp");

  assert(pluginChainAddFromArgumentString(p, testArgs, NULL));
  assertIntEquals(1, p->numPlugins);
  assertIntEquals(PLUGIN_TYPE_INTERNAL, p->plugins[0]->pluginType);
  assertCharStringEquals(kInternalPluginPassthruName,
                         p->plugins[0]->pluginName);
  assertNotNull(p->presets[0]);
  assertCharStringEquals("test preset.fxp", p->presets[0]->presetName);

  freeCharString(testArgs);
  return 0;
}

static int _testAppendPlugin(void) {
  Plugin mock = newPluginMock();
  PluginChain p = getPluginChain();
  assert(pluginChainAppend(p, mock, NULL));
  return 0;
}

static int _testAppendWithNullPlugin(void) {
  PluginChain p = getPluginChain();
  assertFalse(pluginChainAppend(p, NULL, NULL));
  return 0;
}

static int _testAppendWithPreset(void) {
  Plugin mock = newPluginMock();
  PluginChain p = getPluginChain();
  PluginPreset mockPreset = newPluginPresetMock();
  assert(pluginChainAppend(p, mock, mockPreset));
  return 0;
}

static int _testInitializePluginChain(void) {
  Plugin mock = newPluginMock();
  PluginChain p = getPluginChain();
  PluginPreset mockPreset = newPluginPresetMock();

  assert(pluginChainAppend(p, mock, mockPreset));
  pluginChainInitialize(p);
  assert(((PluginMockData)mock->extraData)->isOpen);
  assert(((PluginPresetMockData)mockPreset->extraData)->isOpen);
  assert(((PluginPresetMockData)mockPreset->extraData)->isLoaded);

  return 0;
}

static int _testGetMaximumTailTime(void) {
  Plugin mock = newPluginMock();
  PluginChain p = getPluginChain();
  int maxTailTime = 0;

  assert(pluginChainAppend(p, mock, NULL));
  maxTailTime = pluginChainGetMaximumTailTimeInMs(p);
  assertIntEquals(kPluginMockTailTime, maxTailTime);

  return 0;
}

static int _testPrepareForProcessing(void) {
  Plugin mock = newPluginMock();
  PluginChain p = getPluginChain();

  assert(pluginChainAppend(p, mock, NULL));
  assertIntEquals(RETURN_CODE_SUCCESS, pluginChainInitialize(p));
  pluginChainPrepareForProcessing(p);
  assert(((PluginMockData)mock->extraData)->isPrepared);

  return 0;
}

static int _testProcessPluginChainAudio(void) {
  Plugin mock = newPluginMock();
  PluginChain p = getPluginChain();
  SampleBuffer inBuffer =
      newSampleBuffer(DEFAULT_NUM_CHANNELS, DEFAULT_BLOCKSIZE);
  SampleBuffer outBuffer =
      newSampleBuffer(DEFAULT_NUM_CHANNELS, DEFAULT_BLOCKSIZE);

  assert(pluginChainAppend(p, mock, NULL));
  pluginChainProcessAudio(p, inBuffer, outBuffer);
  assert(((PluginMockData)mock->extraData)->processAudioCalled);

  freeSampleBuffer(inBuffer);
  freeSampleBuffer(outBuffer);
  return 0;
}

static int _testProcessPluginChainAudioRealtime(void) {
  Plugin mock = newPluginMock();
  PluginChain p = getPluginChain();
  SampleBuffer inBuffer =
      newSampleBuffer(DEFAULT_NUM_CHANNELS, DEFAULT_BLOCKSIZE);
  SampleBuffer outBuffer =
      newSampleBuffer(DEFAULT_NUM_CHANNELS, DEFAULT_BLOCKSIZE);
  TaskTimer t = newTaskTimerWithCString("test", "test");

  assert(pluginChainAppend(p, mock, NULL));
  pluginChainSetRealtime(p, true);
  taskTimerStart(t);
  pluginChainProcessAudio(p, inBuffer, outBuffer);
  assertTimeEquals(1000 * DEFAULT_BLOCKSIZE / getSampleRate(), taskTimerStop(t),
                   0.1);
  assert(((PluginMockData)mock->extraData)->processAudioCalled);

  freeTaskTimer(t);
  freeSampleBuffer(inBuffer);
  freeSampleBuffer(outBuffer);
  return 0;
}

static int _testProcessPluginChainMidiEvents(void) {
  Plugin mock = newPluginMock();
  PluginChain p = getPluginChain();
  SampleBuffer inBuffer =
      newSampleBuffer(DEFAULT_NUM_CHANNELS, DEFAULT_BLOCKSIZE);
  SampleBuffer outBuffer =
      newSampleBuffer(DEFAULT_NUM_CHANNELS, DEFAULT_BLOCKSIZE);
  LinkedList list = newLinkedList();
  MidiEvent midi = newMidiEvent();

  linkedListAppend(list, midi);
  assert(pluginChainAppend(p, mock, NULL));
  pluginChainProcessMidi(p, list);
  assert(((PluginMockData)mock->extraData)->processMidiCalled);

  freeMidiEvent(midi);
  freeLinkedList(list);
  freeSampleBuffer(inBuffer);
  freeSampleBuffer(outBuffer);
  return 0;
}

static int _testShutdown(void) {
  Plugin mock = newPluginMock();
  PluginChain p = getPluginChain();

  assert(pluginChainAppend(p, mock, NULL));
  pluginChainShutdown(p);
  assertFalse(((PluginMockData)mock->extraData)->isOpen);

  return 0;
}

TestSuite addPluginChainTests(void);
TestSuite addPluginChainTests(void) {
  TestSuite testSuite = newTestSuite("PluginChain", _pluginChainTestSetup,
                                     _pluginChainTestTeardown);
  addTest(testSuite, "Initialization", _testInitPluginChain);
  addTest(testSuite, "AddFromArgumentStringNull",
          _testAddFromArgumentStringNull);
  addTest(testSuite, "AddFromArgumentStringEmpty",
          _testAddFromArgumentStringEmpty);
  addTest(testSuite, "AddFromArgumentStringEmptyLocation",
          _testAddFromArgumentStringEmptyLocation);
  addTest(testSuite, "AddFromArgumentStringNullLocation",
          _testAddFromArgumentStringNullLocation);
  addTest(testSuite, "AddFromArgumentString", _testAddFromArgumentString);
  addTest(testSuite, "AddFromArgumentStringMultiple",
          _testAddFromArgumentStringMultiple);
  addTest(testSuite, "AddPluginWithPresetFromArgumentString",
          _testAddPluginWithPresetFromArgumentString);
  addTest(testSuite, "AddFromArgumentStringWithPresetSpaces",
          _testAddFromArgumentStringWithPresetSpaces);
  addTest(testSuite, "AppendPlugin", _testAppendPlugin);
  addTest(testSuite, "AppendWithNullPlugin", _testAppendWithNullPlugin);
  addTest(testSuite, "AppendWithPreset", _testAppendWithPreset);
  addTest(testSuite, "InitializePluginChain", _testInitializePluginChain);

  addTest(testSuite, "GetMaximumTailTime", _testGetMaximumTailTime);

  addTest(testSuite, "PrepareForProcessing", _testPrepareForProcessing);
  addTest(testSuite, "ProcessPluginChainAudio", _testProcessPluginChainAudio);
  addTest(testSuite, "ProcessPluginChainAudioRealtime",
          _testProcessPluginChainAudioRealtime);
  addTest(testSuite, "ProcessPluginChainMidiEvents",
          _testProcessPluginChainMidiEvents);

  addTest(testSuite, "Shutdown", _testShutdown);

  return testSuite;
}
