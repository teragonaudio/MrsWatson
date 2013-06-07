#include "unit/TestRunner.h"
#include "plugin/PluginPreset.h"
#include "PluginMock.h"

const char* TEST_PRESET_FILENAME = "test.fxp";

static int _testGuessPluginPresetType(void) {
  CharString c = newCharStringWithCString(TEST_PRESET_FILENAME);
  PluginPreset p = pluginPresetFactory(c);
  assertIntEquals(p->presetType, PRESET_TYPE_FXP);
  freePluginPreset(p);
  freeCharString(c);
  return 0;
}

static int _testGuessPluginPresetTypeInvalid(void) {
  CharString c = newCharStringWithCString("invalid");
  PluginPreset p = pluginPresetFactory(c);
  assertIsNull(p);
  freePluginPreset(p);
  freeCharString(c);
  return 0;
}

static int _testNewObject(void) {
  CharString c = newCharStringWithCString(TEST_PRESET_FILENAME);
  PluginPreset p = pluginPresetFactory(c);
  assertIntEquals(p->presetType, PRESET_TYPE_FXP);
  assertCharStringEquals(p->presetName, TEST_PRESET_FILENAME);
  freePluginPreset(p);
  freeCharString(c);
  return 0;
}

static int _testIsPresetCompatibleWithPlugin(void) {
  CharString c = newCharStringWithCString(TEST_PRESET_FILENAME);
  PluginPreset p = pluginPresetFactory(c);
  Plugin mockPlugin = newPluginMock();

  _pluginPresetSetCompatibleWith(p, PLUGIN_TYPE_INTERNAL);
  assert(pluginPresetIsCompatibleWith(p, mockPlugin));

  freePlugin(mockPlugin);
  freeCharString(c);
  freePluginPreset(p);
  return 0;
}

static int _testIsPresetNotCompatibleWithPlugin(void) {
  CharString c = newCharStringWithCString(TEST_PRESET_FILENAME);
  PluginPreset p = pluginPresetFactory(c);
  Plugin mockPlugin = newPluginMock();

  _pluginPresetSetCompatibleWith(p, PLUGIN_TYPE_VST_2X);
  assertFalse(pluginPresetIsCompatibleWith(p, mockPlugin));

  freePlugin(mockPlugin);
  freeCharString(c);
  freePluginPreset(p);
  return 0;
}

TestSuite addPluginPresetTests(void);
TestSuite addPluginPresetTests(void) {
  TestSuite testSuite = newTestSuite("PluginPreset", NULL, NULL);
  addTest(testSuite, "GuessPluginPresetType", _testGuessPluginPresetType);
  addTest(testSuite, "GuessPluginPresetTypeInvalid", _testGuessPluginPresetTypeInvalid);
  addTest(testSuite, "NewObject", _testNewObject);
  addTest(testSuite, "IsPresetCompatibleWithPlugin", _testIsPresetCompatibleWithPlugin);
  addTest(testSuite, "IsPresetNotCompatibleWithPlugin", _testIsPresetNotCompatibleWithPlugin);
  return testSuite;
}
