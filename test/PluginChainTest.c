#include "TestRunner.h"
#include "PluginChain.h"

static int _testNewPluginChain(void) {
  PluginChain p = newPluginChain();
  assertIntEquals(p->numPlugins, 0);
  assertNotNull(p->plugins);
  assertNotNull(p->presets);
  return 0;
}

static int _testAddPluginsFromArgumentString(void) {
  return 0;
}

static int _testAddPluginsFromArgumentStringWithSpaces(void) {
  return 0;
}

static int _testAddPluginsFromArgumentStringWithPresets(void) {
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
  addTest(testSuite, "AddPluginsFromArgumentString", NULL); // _testAddPluginsFromArgumentString);
  addTest(testSuite, "AddPluginsFromArgumentStringWithSpaces", NULL); // _testAddPluginsFromArgumentStringWithSpaces);
  addTest(testSuite, "AddPluginsFromArgumentStringWithPresets", NULL); // _testAddPluginsFromArgumentStringWithPresets);
  addTest(testSuite, "GetMaximumTailTime", NULL); // _testGetMaximumTailTime);
  addTest(testSuite, "ProcessPluginChainAudio", NULL); // _testProcessPluginChainAudio);
  addTest(testSuite, "ProcessPluginChainMidiEvents", NULL); // _testProcessPluginChainMidiEvents);
  addTest(testSuite, "ClosePluginChain", NULL); // _testClosePluginChain);
  return testSuite;
}
