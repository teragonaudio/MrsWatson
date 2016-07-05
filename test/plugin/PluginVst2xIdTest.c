//
// PluginVst2xIdTest.c - MrsWatson
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

#include "plugin/PluginVst2xId.h"

#include "unit/TestRunner.h"

static int _testNewPluginVst2xId(void) {
  PluginVst2xId id = newPluginVst2xId();
  assertUnsignedLongEquals(ZERO_UNSIGNED_LONG, id->id);
  assertCharStringEquals(PLUGIN_VST2X_ID_UNKNOWN, id->idString);
  freePluginVst2xId(id);
  return 0;
}

static int _testNewPluginVst2xIdWithIntId(void) {
  PluginVst2xId id = newPluginVst2xIdWithId(0x61626364);
  assertUnsignedLongEquals(0x61626364l, id->id);
  assertCharStringEquals("abcd", id->idString);
  freePluginVst2xId(id);
  return 0;
}

static int _testNewPluginVst2xIdWithZeroIntId(void) {
  PluginVst2xId id = newPluginVst2xIdWithId(0);
  assertUnsignedLongEquals(ZERO_UNSIGNED_LONG, id->id);
  assertCharStringEquals(EMPTY_STRING, id->idString);
  freePluginVst2xId(id);
  return 0;
}

static int _testNewPluginVst2xIdWithStringId(void) {
  CharString c = newCharStringWithCString("abcd");
  PluginVst2xId id = newPluginVst2xIdWithStringId(c);
  assertUnsignedLongEquals(0x61626364l, id->id);
  assertCharStringEquals(c->data, id->idString);
  freePluginVst2xId(id);
  freeCharString(c);
  return 0;
}

static int _testNewPluginVst2xIdWithEmptyStringId(void) {
  CharString empty = newCharStringWithCString(EMPTY_STRING);
  PluginVst2xId id = newPluginVst2xIdWithStringId(empty);
  assertUnsignedLongEquals(ZERO_UNSIGNED_LONG, id->id);
  assertCharStringEquals(PLUGIN_VST2X_ID_UNKNOWN, id->idString);
  freePluginVst2xId(id);
  freeCharString(empty);
  return 0;
}

static int _testNewPluginVst2xIdWithNullStringId(void) {
  PluginVst2xId id = newPluginVst2xIdWithStringId(NULL);
  assertUnsignedLongEquals(ZERO_UNSIGNED_LONG, id->id);
  assertCharStringEquals(PLUGIN_VST2X_ID_UNKNOWN, id->idString);
  freePluginVst2xId(id);
  return 0;
}

static int _testNewPluginVst2xIdWithInvalidStringId(void) {
  CharString c = newCharStringWithCString("a");
  PluginVst2xId id = newPluginVst2xIdWithStringId(c);
  assertUnsignedLongEquals(ZERO_UNSIGNED_LONG, id->id);
  assertCharStringEquals(PLUGIN_VST2X_ID_UNKNOWN, id->idString);
  freePluginVst2xId(id);
  freeCharString(c);
  return 0;
}

TestSuite addPluginVst2xIdTests(void);
TestSuite addPluginVst2xIdTests(void) {
  TestSuite testSuite = newTestSuite("PluginVst2xId", NULL, NULL);

  addTest(testSuite, "NewPluginVst2xId", _testNewPluginVst2xId);
  addTest(testSuite, "NewPluginVst2xIdWithIntId",
          _testNewPluginVst2xIdWithIntId);
  addTest(testSuite, "NewPluginVst2xIdWithZeroIntId",
          _testNewPluginVst2xIdWithZeroIntId);
  addTest(testSuite, "NewPluginVst2xIdWithStringId",
          _testNewPluginVst2xIdWithStringId);
  addTest(testSuite, "NewPluginVst2xIdWithNullStringId",
          _testNewPluginVst2xIdWithNullStringId);
  addTest(testSuite, "NewPluginVst2xIdWithEmptyStringId",
          _testNewPluginVst2xIdWithEmptyStringId);
  addTest(testSuite, "NewPluginVst2xIdWithInvalidStringId",
          _testNewPluginVst2xIdWithInvalidStringId);

  return testSuite;
}
