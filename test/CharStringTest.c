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

static int _testCharStringEqualsSameCString(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  _assert(isCharStringEqualToCString(c, TEST_STRING, false));
  return 0;
}

static int _testCharStringNotEqualToDifferentCString(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  _assertFalse(isCharStringEqualToCString(c, OTHER_TEST_STRING, false));
  return 0;
}

static int _testCharStringEqualsSameCStringInsensitive(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  _assert(isCharStringEqualToCString(c, TEST_STRING_CAPS, true));
  return 0;
}

static int _testCharStringNotEqualsCStringInsensitive(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  _assertFalse(isCharStringEqualToCString(c, TEST_STRING_CAPS, false));
  return 0;
}

static int _testCharStringEqualsCStringNull(void) {
  CharString c = newCharString();
  _assertFalse(isCharStringEqualToCString(c, NULL, false));
  return 0;
}

TestSuite addCharStringTests(void);
TestSuite addCharStringTests(void) {
  TestSuite testSuite = newTestSuite("CharString", NULL, NULL);

  addTest(testSuite, "New CharString", _testNewCharString);
  addTest(testSuite, "New object with capacity", _testNewCharStringWithCapacity);
  addTest(testSuite, "Clear string", _testClearCharString);
  addTest(testSuite, "Copy to CharString", _testCopyToCharString);
  addTest(testSuite, "Copy CharStrings", _testCopyCharStrings);
  addTest(testSuite, "Empty string is empty", _testIsEmptyStringEmpty);
  addTest(testSuite, "Null is empty", _testIsNullEmptyString);
  addTest(testSuite, "Regular string is not empty", _testIsRegularStringNotEmpty);

  addTest(testSuite, "Equals same string", _testCharStringEqualsSameString);
  addTest(testSuite, "Does not equal different string", _testCharStringDoesEqualDifferentString);
  addTest(testSuite, "Equals same string with case insensitive", _testCharStringEqualsSameStringInsensitive);
  addTest(testSuite, "Does not equal same string with different case", _testCharStringDoesNotEqualSameStringInsensitive);
  addTest(testSuite, "Equals null", _testCharStringEqualsNull);

  addTest(testSuite, "Equals same C-string", _testCharStringEqualsSameCString);
  addTest(testSuite, "Does not equal different C-string", _testCharStringNotEqualToDifferentCString);
  addTest(testSuite, "Equals same C-string with case insensitive", _testCharStringEqualsSameCStringInsensitive);
  addTest(testSuite, "Does not equal same C-string with different case", _testCharStringNotEqualsCStringInsensitive);
  addTest(testSuite, "Equals C-string null", _testCharStringEqualsCStringNull);

  return testSuite;
}