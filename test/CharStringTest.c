#include "TestRunner.h"
#include "CharString.h"

static char *const TEST_STRING = "test string";
static char *const TEST_STRING_CAPS = "TEST STRING";
static char *const OTHER_TEST_STRING = "other test string";

static int _testNewCharString(void) {
  CharString c = newCharString();
  _assertIntEquals(c->capacity, STRING_LENGTH_DEFAULT);
  _assertCharStringEquals(c, "");
  return 0;
}

static int _testNewCharStringWithCapacity(void) {
  CharString c = newCharStringWithCapacity(123);
  _assertIntEquals(c->capacity, 123);
  _assertCharStringEquals(c, "");
  return 0;
}

static int _testClearCharString(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  clearCharString(c);
  _assertIntEquals(c->capacity, STRING_LENGTH_DEFAULT);
  _assertCharStringEquals(c, "");
  return 0;
}

static int _testCopyToCharString(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  _assertIntEquals(c->capacity, STRING_LENGTH_DEFAULT);
  _assertCharStringEquals(c, TEST_STRING);
  return 0;
}

static int _testCopyCharStrings(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  copyToCharString(c, TEST_STRING);
  copyCharStrings(c2, c);
  _assertCharStringEquals(c, c2->data);
  return 0;
}

static int _testIsEmptyStringEmpty(void) {
  CharString c = newCharString();
  _assert(isCharStringEmpty(c));
  return 0;
}

static int _testIsNullEmptyString(void) {
  _assert(isCharStringEmpty(NULL));
  return 0;
}

static int _testIsRegularStringNotEmpty(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  _assertFalse(isCharStringEmpty(c));
  return 0;
}

static int _testCharStringEqualsSameString(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  copyToCharString(c, TEST_STRING);
  copyToCharString(c2, TEST_STRING);
  _assert(isCharStringEqualTo(c, c2, false));
  return 0;
}

static int _testCharStringDoesEqualDifferentString(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  copyToCharString(c, TEST_STRING);
  copyToCharString(c2, OTHER_TEST_STRING);
  _assertFalse(isCharStringEqualTo(c, c2, false));
  return 0;
}

static int _testCharStringEqualsSameStringInsensitive(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  copyToCharString(c, TEST_STRING);
  copyToCharString(c2, TEST_STRING_CAPS);
  _assert(isCharStringEqualTo(c, c2, true));
  return 0;
}

static int _testCharStringDoesNotEqualSameStringInsensitive(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  copyToCharString(c, TEST_STRING);
  copyToCharString(c2, TEST_STRING_CAPS);
  _assertFalse(isCharStringEqualTo(c, c2, false));
  return 0;
}

static int _testCharStringEqualsNull(void) {
  CharString c = newCharString();
  _assertFalse(isCharStringEqualTo(c, NULL, false));
  return 0;
}

static int _testCharStringEqualsCString(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  _assert(isCharStringEqualToCString(c, TEST_STRING, true));
  return 0;
}

void runCharStringTests(void);
void runCharStringTests(void) {
  _startTestSection();
  _runTest("New object", _testNewCharString, emptySetup, emptyTeardown);
  _runTest("New object with capacity", _testNewCharStringWithCapacity, emptySetup, emptyTeardown);
  _runTest("Clear string", _testClearCharString, emptySetup, emptyTeardown);
  _runTest("Copy to CharString", _testCopyToCharString, emptySetup, emptyTeardown);
  _runTest("Copy CharStrings", _testCopyCharStrings, emptySetup, emptyTeardown);
  _runTest("Empty string is empty", _testIsEmptyStringEmpty, emptySetup, emptyTeardown);
  _runTest("Null is empty", _testIsNullEmptyString, emptySetup, emptyTeardown);
  _runTest("Regular string is not empty", _testIsRegularStringNotEmpty, emptySetup, emptyTeardown);
  _runTest("CharString equals same string", _testCharStringEqualsSameString, emptySetup, emptyTeardown);
  _runTest("CharString does not equal different string", _testCharStringDoesEqualDifferentString, emptySetup, emptyTeardown);
  _runTest("CharString equals same string with case insensitive", _testCharStringEqualsSameStringInsensitive, emptySetup, emptyTeardown);
  _runTest("CharString does equal same string with different case", _testCharStringDoesNotEqualSameStringInsensitive, emptySetup, emptyTeardown);
  _runTest("CharString equals null", _testCharStringEqualsNull, emptySetup, emptyTeardown);
  _runTest("CharString equals C-string", _testCharStringEqualsCString, emptySetup, emptyTeardown);
}