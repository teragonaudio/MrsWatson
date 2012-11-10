#include "unit/TestRunner.h"
#include "plugin/PluginPreset.h"

const char* TEST_PRESET_FILENAME = "test.fxp";

static int _testGuessPluginPresetType(void) {
  assertIntEquals(guessPluginPresetType(newCharStringWithCString(TEST_PRESET_FILENAME)), PRESET_TYPE_FXP);
  return 0;
}

static int _testGuessPluginPresetTypeInvalid(void) {
  assertIntEquals(guessPluginPresetType(newCharStringWithCString("invalid")), PRESET_TYPE_INVALID);
  return 0;
}

static int _testNewObject(void) {
  PluginPreset p = newPluginPreset(PRESET_TYPE_FXP, newCharStringWithCString(TEST_PRESET_FILENAME));
  assertIntEquals(p->presetType, PRESET_TYPE_FXP);
  assertCharStringEquals(p->presetName, TEST_PRESET_FILENAME);
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
