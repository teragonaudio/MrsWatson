#include "unit/TestRunner.h"
#include "plugin/PluginVst2xId.h"

static int _testNewPluginVst2xId(void) {
  PluginVst2xId id = newPluginVst2xId();
  assertUnsignedLongEquals(id->id, 0l);
  assertCharStringEquals(id->idString, kPluginVst2xIdUnknown);
  freePluginVst2xId(id);
  return 0;
}

static int _testNewPluginVst2xIdWithIntId(void) {
  PluginVst2xId id = newPluginVst2xIdWithId(0x61626364);
  assertUnsignedLongEquals(id->id, 0x61626364l);
  assertCharStringEquals(id->idString, "abcd");
  freePluginVst2xId(id);
  return 0;
}

static int _testNewPluginVst2xIdWithZeroIntId(void) {
  PluginVst2xId id = newPluginVst2xIdWithId(0);
  assertUnsignedLongEquals(id->id, 0l);
  assertCharStringEquals(id->idString, EMPTY_STRING);
  freePluginVst2xId(id);
  return 0;
}

static int _testNewPluginVst2xIdWithStringId(void) {
  CharString c = newCharStringWithCString("abcd");
  PluginVst2xId id = newPluginVst2xIdWithStringId(c);
  assertUnsignedLongEquals(id->id, 0x61626364l);
  assertCharStringEquals(id->idString, c->data);
  freePluginVst2xId(id);
  freeCharString(c);
  return 0;
}

static int _testNewPluginVst2xIdWithEmptyStringId(void) {
  CharString empty = newCharStringWithCString(EMPTY_STRING);
  PluginVst2xId id = newPluginVst2xIdWithStringId(empty);
  assertUnsignedLongEquals(id->id, 0l);
  assertCharStringEquals(id->idString, kPluginVst2xIdUnknown);
  freePluginVst2xId(id);
  freeCharString(empty);
  return 0;
}

static int _testNewPluginVst2xIdWithNullStringId(void) {
  PluginVst2xId id = newPluginVst2xIdWithStringId(NULL);
  assertUnsignedLongEquals(id->id, 0l);
  assertCharStringEquals(id->idString, kPluginVst2xIdUnknown);
  freePluginVst2xId(id);
  return 0;
}

static int _testNewPluginVst2xIdWithInvalidStringId(void) {
  CharString c = newCharStringWithCString("a");
  PluginVst2xId id = newPluginVst2xIdWithStringId(c);
  assertUnsignedLongEquals(id->id, 0l);
  assertCharStringEquals(id->idString, kPluginVst2xIdUnknown);
  freePluginVst2xId(id);
  freeCharString(c);
  return 0;
}

TestSuite addPluginVst2xIdTests(void);
TestSuite addPluginVst2xIdTests(void) {
  TestSuite testSuite = newTestSuite("PluginVst2xId", NULL, NULL);

  addTest(testSuite, "NewPluginVst2xId", _testNewPluginVst2xId);
  addTest(testSuite, "NewPluginVst2xIdWithIntId", _testNewPluginVst2xIdWithIntId);
  addTest(testSuite, "NewPluginVst2xIdWithZeroIntId", _testNewPluginVst2xIdWithZeroIntId);
  addTest(testSuite, "NewPluginVst2xIdWithStringId", _testNewPluginVst2xIdWithStringId);
  addTest(testSuite, "NewPluginVst2xIdWithNullStringId", _testNewPluginVst2xIdWithNullStringId);
  addTest(testSuite, "NewPluginVst2xIdWithEmptyStringId", _testNewPluginVst2xIdWithEmptyStringId);
  addTest(testSuite, "NewPluginVst2xIdWithInvalidStringId",_testNewPluginVst2xIdWithInvalidStringId);

  return testSuite;
}
