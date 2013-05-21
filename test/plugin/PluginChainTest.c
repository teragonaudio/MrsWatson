#include "unit/TestRunner.h"
#include "plugin/PluginChain.h"
#include "plugin/PluginPassthru.h"

static int _testNewPluginChain(void) {
  PluginChain p = newPluginChain();
  assertIntEquals(p->numPlugins, 0);
  assertNotNull(p->plugins);
  assertNotNull(p->presets);
  freePluginChain(p);
  return 0;
}

static int _testAddPluginFromArgumentStringNull(void) {
  PluginChain p = newPluginChain();
  CharString c = newCharStringWithCString("/");

  assertFalse(pluginChainAddFromArgumentString(p, NULL, c));
  assertIntEquals(p->numPlugins, 0);

  freePluginChain(p);
  freeCharString(c);
  return 0;
}

static int _testAddPluginFromArgumentStringEmpty(void) {
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

static int _testAddPluginFromArgumentStringEmptyLocation(void) {
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

static int _testAddPluginFromArgumentStringNullLocation(void) {
  PluginChain p = newPluginChain();
  CharString c = newCharStringWithCString(kInternalPluginPassthruName);

  assert(pluginChainAddFromArgumentString(p, c, NULL));
  assertIntEquals(p->numPlugins, 1);

  freePluginChain(p);
  freeCharString(c);
  return 0;
}

static int _testAddPluginFromArgumentString(void) {
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

static int _testAddPluginsFromArgumentString(void) {
  PluginChain p = newPluginChain();
  CharString testArgs = newCharStringWithCString(kInternalPluginPassthruName);
  int i;

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

static int _testAddPluginFromArgumentStringWithPresetSpaces(void) {
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
  addTest(testSuite, "AddPluginFromArgumentStringWithPresetSpaces", _testAddPluginFromArgumentStringWithPresetSpaces);
  addTest(testSuite, "GetMaximumTailTime", NULL); // _testGetMaximumTailTime);
  addTest(testSuite, "ProcessPluginChainAudio", NULL); // _testProcessPluginChainAudio);
  addTest(testSuite, "ProcessPluginChainMidiEvents", NULL); // _testProcessPluginChainMidiEvents);
  addTest(testSuite, "ClosePluginChain", NULL); // _testClosePluginChain);
  return testSuite;
}
