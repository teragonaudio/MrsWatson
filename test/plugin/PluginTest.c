//
// PluginTest.c - MrsWatson
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

#include "plugin/Plugin.h"

#include "unit/TestRunner.h"

static int _testPluginFactory(void) {
  CharString silence = newCharStringWithCString("mrs_silence");
  CharString pluginRoot = newCharString();
  Plugin p = pluginFactory(silence, pluginRoot);

  assertNotNull(p);
  assertIntEquals(PLUGIN_TYPE_INTERNAL, p->interfaceType);
  assertCharStringEquals(silence->data, p->pluginName);

  freeCharString(silence);
  freeCharString(pluginRoot);
  freePlugin(p);
  return 0;
}

static int _testPluginFactoryInvalidPlugin(void) {
  CharString invalid = newCharStringWithCString("invalid");
  CharString pluginRoot = newCharString();
  Plugin p = pluginFactory(invalid, pluginRoot);

  assertIsNull(p);

  freeCharString(invalid);
  freeCharString(pluginRoot);
  freePlugin(p);
  return 0;
}

static int _testPluginFactoryNullPluginName(void) {
  CharString pluginRoot = newCharString();
  Plugin p = pluginFactory(NULL, pluginRoot);

  assertIsNull(p);

  freeCharString(pluginRoot);
  freePlugin(p);
  return 0;
}

static int _testPluginFactoryEmptyPluginName(void) {
  CharString invalid = newCharString();
  CharString pluginRoot = newCharString();
  Plugin p = pluginFactory(invalid, pluginRoot);

  assertIsNull(p);

  freeCharString(invalid);
  freeCharString(pluginRoot);
  freePlugin(p);
  return 0;
}

static int _testPluginFactoryNullRoot(void) {
  CharString silence = newCharStringWithCString("mrs_silence");
  Plugin p = pluginFactory(silence, NULL);

  assertNotNull(p);
  assertIntEquals(PLUGIN_TYPE_INTERNAL, p->interfaceType);
  assertCharStringEquals(silence->data, p->pluginName);

  freeCharString(silence);
  freePlugin(p);
  return 0;
}

static int _testFreeNullPlugin(void) {
  freePlugin(NULL);
  return 0;
}

TestSuite addPluginTests(void);
TestSuite addPluginTests(void) {
  TestSuite testSuite = newTestSuite("Plugin", NULL, NULL);
  addTest(testSuite, "PluginFactory", _testPluginFactory);
  addTest(testSuite, "PluginFactoryInvalidPlugin",
          _testPluginFactoryInvalidPlugin);
  addTest(testSuite, "PluginFactoryNullPluginName",
          _testPluginFactoryNullPluginName);
  addTest(testSuite, "PluginFactoryEmptyPluginName",
          _testPluginFactoryEmptyPluginName);
  addTest(testSuite, "PluginFactoryNullRoot", _testPluginFactoryNullRoot);
  addTest(testSuite, "FreeNullPlugin", _testFreeNullPlugin);
  return testSuite;
}
