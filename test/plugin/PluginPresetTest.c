#include "unit/TestRunner.h"
#include "plugin/PluginPreset.h"

const char* TEST_PRESET_FILENAME = "test.fxp";

static int _testGuessPluginPresetType(void) {
  CharString c = newCharStringWithCString(TEST_PRESET_FILENAME);
  assertIntEquals(pluginPresetGuessType(c), PRESET_TYPE_FXP);
  freeCharString(c);
  return 0;
}

static int _testGuessPluginPresetTypeInvalid(void) {
  CharString c = newCharStringWithCString("invalid");
  assertIntEquals(pluginPresetGuessType(c), PRESET_TYPE_INVALID);
  freeCharString(c);
  return 0;
}

static int _testNewObject(void) {
  CharString c = newCharStringWithCString(TEST_PRESET_FILENAME);
  PluginPreset p = newPluginPreset(PRESET_TYPE_FXP, c);
  assertIntEquals(p->presetType, PRESET_TYPE_FXP);
  assertCharStringEquals(p->presetName, TEST_PRESET_FILENAME);
  freePluginPreset(p);
  freeCharString(c);
  return 0;
}

static int _testIsPresetCompatibleWithPlugin(void) {
  return 0;
}

TestSuite addPluginPresetTests(void);
TestSuite addPluginPresetTests(void) {
  TestSuite testSuite = newTestSuite("PluginPreset", NULL, NULL);
  addTest(testSuite, "GuessPluginPresetType", _testGuessPluginPresetType);
  addTest(testSuite, "GuessPluginPresetTypeInvalid", _testGuessPluginPresetTypeInvalid);
  addTest(testSuite, "NewObject", _testNewObject);
  addTest(testSuite, "IsPresetCompatibleWithPlugin", NULL); // _testIsPresetCompatibleWithPlugin);
  return testSuite;
}
