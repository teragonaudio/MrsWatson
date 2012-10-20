#include "TestRunner.h"
#include "PluginPreset.h"

static int _testGuessPluginPresetType(void) {
  return 0;
}

static int _testGuessPluginPresetTypeInvalid(void) {
  return 0;
}

static int _testNewObject(void) {
  return 0;
}

static int _testIsPresetCompatibleWithPlugin(void) {
  return 0;
}

TestSuite addPluginPresetTests(void);
TestSuite addPluginPresetTests(void) {
  TestSuite testSuite = newTestSuite("PluginPreset", NULL, NULL);
  addTest(testSuite, "GuessPluginPresetType", NULL); // _testGuessPluginPresetType);
  addTest(testSuite, "GuessPluginPresetTypeInvalid", NULL); // _testGuessPluginPresetTypeInvalid);
  addTest(testSuite, "NewObject", NULL); // _testNewObject);
  addTest(testSuite, "IsPresetCompatibleWithPlugin", NULL); // _testIsPresetCompatibleWithPlugin);
  return testSuite;
}
