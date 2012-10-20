#include "TestRunner.h"
#include "CharString.h"

static char *const TEST_STRING = "test string";
static char *const TEST_STRING_CAPS = "TEST STRING";
static char *const OTHER_TEST_STRING = "other test string";

static int _testNewCharString(void) {
  CharString c = newCharString();
  assertIntEquals(c->capacity, STRING_LENGTH_DEFAULT);
  assertCharStringEquals(c, "");
  return 0;
}

static int _testNewCharStringWithCapacity(void) {
  CharString c = newCharStringWithCapacity(123);
  assertIntEquals(c->capacity, 123);
  assertCharStringEquals(c, "");
  return 0;
}

static int _testClearCharString(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  clearCharString(c);
  assertIntEquals(c->capacity, STRING_LENGTH_DEFAULT);
  assertCharStringEquals(c, "");
  return 0;
}

static int _testCopyToCharString(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  assertIntEquals(c->capacity, STRING_LENGTH_DEFAULT);
  assertCharStringEquals(c, TEST_STRING);
  return 0;
}

static int _testCopyCharStrings(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  copyToCharString(c, TEST_STRING);
  copyCharStrings(c2, c);
  assertCharStringEquals(c, c2->data);
  return 0;
}

static int _testIsEmptyStringEmpty(void) {
  CharString c = newCharString();
  assert(isCharStringEmpty(c));
  return 0;
}

static int _testIsNullEmptyString(void) {
  assert(isCharStringEmpty(NULL));
  return 0;
}

static int _testIsRegularStringNotEmpty(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  assertFalse(isCharStringEmpty(c));
  return 0;
}

static int _testCharStringEqualsSameString(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  copyToCharString(c, TEST_STRING);
  copyToCharString(c2, TEST_STRING);
  assert(isCharStringEqualTo(c, c2, false));
  return 0;
}

static int _testCharStringDoesEqualDifferentString(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  copyToCharString(c, TEST_STRING);
  copyToCharString(c2, OTHER_TEST_STRING);
  assertFalse(isCharStringEqualTo(c, c2, false));
  return 0;
}

static int _testCharStringEqualsSameStringInsensitive(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  copyToCharString(c, TEST_STRING);
  copyToCharString(c2, TEST_STRING_CAPS);
  assert(isCharStringEqualTo(c, c2, true));
  return 0;
}

static int _testCharStringDoesNotEqualSameStringInsensitive(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  copyToCharString(c, TEST_STRING);
  copyToCharString(c2, TEST_STRING_CAPS);
  assertFalse(isCharStringEqualTo(c, c2, false));
  return 0;
}

static int _testCharStringEqualsNull(void) {
  CharString c = newCharString();
  assertFalse(isCharStringEqualTo(c, NULL, false));
  return 0;
}

static int _testCharStringEqualsSameCString(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  assert(isCharStringEqualToCString(c, TEST_STRING, false));
  return 0;
}

static int _testCharStringNotEqualToDifferentCString(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  assertFalse(isCharStringEqualToCString(c, OTHER_TEST_STRING, false));
  return 0;
}

static int _testCharStringEqualsSameCStringInsensitive(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  assert(isCharStringEqualToCString(c, TEST_STRING_CAPS, true));
  return 0;
}

static int _testCharStringNotEqualsCStringInsensitive(void) {
  CharString c = newCharString();
  copyToCharString(c, TEST_STRING);
  assertFalse(isCharStringEqualToCString(c, TEST_STRING_CAPS, false));
  return 0;
}

static int _testCharStringEqualsCStringNull(void) {
  CharString c = newCharString();
  assertFalse(isCharStringEqualToCString(c, NULL, false));
  return 0;
}

TestSuite addCharStringTests(void);
TestSuite addCharStringTests(void) {
  TestSuite testSuite = newTestSuite("CharString", NULL, NULL);

  addTest(testSuite, "NewObject", _testNewCharString);
  addTest(testSuite, "NewObjectWithCapacity", _testNewCharStringWithCapacity);
  addTest(testSuite, "ClearString", _testClearCharString);
  addTest(testSuite, "CopyToCharString", _testCopyToCharString);
  addTest(testSuite, "CopyCharStrings", _testCopyCharStrings);
  addTest(testSuite, "EmptyStringIsEmpty", _testIsEmptyStringEmpty);
  addTest(testSuite, "NullIsEmpty", _testIsNullEmptyString);
  addTest(testSuite, "RegularStringIsNotEmpty", _testIsRegularStringNotEmpty);

  addTest(testSuite, "EqualsSameString", _testCharStringEqualsSameString);
  addTest(testSuite, "DoesNotEqualDifferentString", _testCharStringDoesEqualDifferentString);
  addTest(testSuite, "EqualsSameStringWithCaseInsensitive", _testCharStringEqualsSameStringInsensitive);
  addTest(testSuite, "DoesNotEqualSameStringWithDifferentCase", _testCharStringDoesNotEqualSameStringInsensitive);
  addTest(testSuite, "EqualsNull", _testCharStringEqualsNull);

  addTest(testSuite, "EqualsSameCString", _testCharStringEqualsSameCString);
  addTest(testSuite, "DoesNotEqualDifferentCString", _testCharStringNotEqualToDifferentCString);
  addTest(testSuite, "EqualsSameCStringWithCaseInsensitive", _testCharStringEqualsSameCStringInsensitive);
  addTest(testSuite, "DoesNotEqualSameCStringWithDifferentCase", _testCharStringNotEqualsCStringInsensitive);
  addTest(testSuite, "EqualsCStringNull", _testCharStringEqualsCStringNull);

  return testSuite;
}
