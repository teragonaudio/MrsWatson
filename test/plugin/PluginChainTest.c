#include "TestRunner.h"
#include "PluginChain.h"

static const char* TEST_PLUGIN_NAME = "mrswatson:passthru";
static const char* TEST_PRESET_NAME = "testPreset.fxp";

static int _testNewPluginChain(void) {
  PluginChain p = newPluginChain();
  assertIntEquals(p->numPlugins, 0);
  assertNotNull(p->plugins);
  assertNotNull(p->presets);
  return 0;
}

static int _testAddPluginFromArgumentStringNull(void) {
  PluginChain p = newPluginChain();
  assertFalse(addPluginsFromArgumentString(p, NULL, newCharStringWithCString("/")));
  assertIntEquals(p->numPlugins, 0);
  return 0;
}

static int _testAddPluginFromArgumentStringEmpty(void) {
  PluginChain p = newPluginChain();
  assertFalse(addPluginsFromArgumentString(p, newCharString(), newCharStringWithCString("/")));
  assertIntEquals(p->numPlugins, 0);
  return 0;
}

static int _testAddPluginFromArgumentStringEmptyLocation(void) {
  PluginChain p = newPluginChain();
  assert(addPluginsFromArgumentString(p, newCharStringWithCString(TEST_PLUGIN_NAME), newCharString()));
  assertIntEquals(p->numPlugins, 1);
  return 0;
}

static int _testAddPluginFromArgumentStringNullLocation(void) {
  PluginChain p = newPluginChain();
  assert(addPluginsFromArgumentString(p, newCharStringWithCString(TEST_PLUGIN_NAME), NULL));
  assertIntEquals(p->numPlugins, 1);
  return 0;
}

static int _testAddPluginFromArgumentString(void) {
  PluginChain p = newPluginChain();
  CharString testArgs = newCharStringWithCString(TEST_PLUGIN_NAME);
  assert(addPluginsFromArgumentString(p, testArgs, NULL));
  assertIntEquals(p->numPlugins, 1);
  assertNotNull(p->plugins[0]);
  assertIntEquals(p->plugins[0]->pluginType, PLUGIN_TYPE_INTERNAL);
  assertCharStringEquals(p->plugins[0]->pluginName, "passthru");
  return 0;
}

static int _testAddPluginsFromArgumentString(void) {
  PluginChain p = newPluginChain();
  CharString testArgs = newCharStringWithCString(TEST_PLUGIN_NAME);
  int i;

  assert(addPluginsFromArgumentString(p, testArgs, NULL));
  assert(addPluginsFromArgumentString(p, testArgs, NULL));
  assertIntEquals(p->numPlugins, 2);
  for(i = 0; i < p->numPlugins; i++) {
    assertNotNull(p->plugins[i]);
    assertIntEquals(p->plugins[i]->pluginType, PLUGIN_TYPE_INTERNAL);
    assertCharStringEquals(p->plugins[i]->pluginName, "passthru");
  }

  return 0;
}

static int _testAddPluginWithPresetFromArgumentString(void) {
  PluginChain p = newPluginChain();
  CharString testArgs = newCharStringWithCString("mrswatson:passthru,testPreset.fxp");
  assert(addPluginsFromArgumentString(p, testArgs, NULL));
  assertIntEquals(p->numPlugins, 1);
  assertIntEquals(p->plugins[0]->pluginType, PLUGIN_TYPE_INTERNAL);
  assertCharStringEquals(p->plugins[0]->pluginName, "passthru");
  assertNotNull(p->presets[0]);
  assertCharStringEquals(p->presets[0]->presetName, "testPreset.fxp");
  return 0;
}

static int _testAddPluginFromArgumentStringWithSpaces(void) {
  // Hmm, I'm not sure how to test this without some really ugly hacks to Plugin.c
  return 0;
}

static int _testAddPluginFromArgumentStringWithPresetSpaces(void) {
  PluginChain p = newPluginChain();
  CharString testArgs = newCharStringWithCString("mrswatson:passthru,test preset.fxp");
  assert(addPluginsFromArgumentString(p, testArgs, NULL));
  assertIntEquals(p->numPlugins, 1);
  assertIntEquals(p->plugins[0]->pluginType, PLUGIN_TYPE_INTERNAL);
  assertCharStringEquals(p->plugins[0]->pluginName, "passthru");
  assertNotNull(p->presets[0]);
  assertCharStringEquals(p->presets[0]->presetName, "test preset.fxp");
  return 0;
}

static int _testGetMaximumTailTime(void) {
  return 0;
}

static int _testProcessPluginChainAudio(void) {
  return 0;
}

static int _testProcessPluginChainMidiEvents(void) {
  return 0;
}

static int _testClosePluginChain(void) {
  return 0;
}

TestSuite addPluginChainTests(void);
TestSuite addPluginChainTests(void) {
  TestSuite testSuite = newTestSuite("PluginChain", NULL, NULL);
  addTest(testSuite, "NewObject", _testNewPluginChain);
  addTest(testSuite, "AddPluginFromArgumentStringNull", _testAddPluginFromArgumentStringNull);
  addTest(testSuite, "AddPluginFromArgumentStringEmpty", _testAddPluginFromArgumentStringEmpty);
  addTest(testSuite, "AddPluginFromArgumentStringEmptyLocation", _testAddPluginFromArgumentStringEmptyLocation);
  addTest(testSuite, "AddPluginFromArgumentStringNullLocation", _testAddPluginFromArgumentStringNullLocation);
  addTest(testSuite, "AddPluginFromArgumentString", _testAddPluginFromArgumentString);
  addTest(testSuite, "AddPluginsFromArgumentString", _testAddPluginsFromArgumentString);
  addTest(testSuite, "AddPluginWithPresetFromArgumentString", _testAddPluginWithPresetFromArgumentString);
  addTest(testSuite, "AddPluginFromArgumentStringWithSpaces", NULL); //_testAddPluginFromArgumentStringWithSpaces);
  addTest(testSuite, "AddPluginFromArgumentStringWithPresetSpaces", _testAddPluginFromArgumentStringWithPresetSpaces);
  addTest(testSuite, "GetMaximumTailTime", NULL); // _testGetMaximumTailTime);
  addTest(testSuite, "ProcessPluginChainAudio", NULL); // _testProcessPluginChainAudio);
  addTest(testSuite, "ProcessPluginChainMidiEvents", NULL); // _testProcessPluginChainMidiEvents);
  addTest(testSuite, "ClosePluginChain", NULL); // _testClosePluginChain);
  return testSuite;
}
