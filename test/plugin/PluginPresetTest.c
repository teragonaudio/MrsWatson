//
// PluginPresetTest.c - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include "plugin/PluginPreset.h"

#include "PluginMock.h"

#include "unit/TestRunner.h"

const char *TEST_PRESET_FILENAME = "test.fxp";

static int _testGuessPluginPresetType(void) {
  CharString c = newCharStringWithCString(TEST_PRESET_FILENAME);
  PluginPreset p = pluginPresetFactory(c);
  assertIntEquals(PRESET_TYPE_FXP, p->presetType);
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
  assertCharStringEquals(TEST_PRESET_FILENAME, p->presetName);
  freePluginPreset(p);
  freeCharString(c);
  return 0;
}

static int _testIsPresetCompatibleWithPlugin(void) {
  CharString c = newCharStringWithCString(TEST_PRESET_FILENAME);
  PluginPreset p = pluginPresetFactory(c);
  Plugin mockPlugin = newPluginMock();

  pluginPresetSetCompatibleWith(p, PLUGIN_TYPE_INTERNAL);
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

  pluginPresetSetCompatibleWith(p, PLUGIN_TYPE_VST_2X);
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
  addTest(testSuite, "GuessPluginPresetTypeInvalid",
          _testGuessPluginPresetTypeInvalid);
  addTest(testSuite, "NewObject", _testNewObject);
  addTest(testSuite, "IsPresetCompatibleWithPlugin",
          _testIsPresetCompatibleWithPlugin);
  addTest(testSuite, "IsPresetNotCompatibleWithPlugin",
          _testIsPresetNotCompatibleWithPlugin);
  return testSuite;
}
