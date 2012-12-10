#include "unit/TestRunner.h"
#include "base/CharString.h"

static char *const TEST_STRING = "test string";
static char *const TEST_STRING_CAPS = "TEST STRING";
static char *const OTHER_TEST_STRING = "other test string";

static int _testNewCharString(void) {
  CharString c = newCharString();
  assertIntEquals(c->length, kCharStringLengthDefault);
  assertCharStringEquals(c, "");
  return 0;
}

static int _testNewCharStringWithCapacity(void) {
  CharString c = newCharStringWithCapacity(123);
  assertIntEquals(c->length, 123);
  assertCharStringEquals(c, "");
  return 0;
}

static int _testClearCharString(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringClear(c);
  assertIntEquals(c->length, kCharStringLengthDefault);
  assertCharStringEquals(c, "");
  return 0;
}

static int _testCopyToCharString(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  assertIntEquals(c->length, kCharStringLengthDefault);
  assertCharStringEquals(c, TEST_STRING);
  return 0;
}

static int _testCopyCharStrings(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringCopy(c2, c);
  assertCharStringEquals(c, c2->data);
  return 0;
}

static int _testIsEmptyStringEmpty(void) {
  CharString c = newCharString();
  assert(charStringIsEmpty(c));
  return 0;
}

static int _testIsNullEmptyString(void) {
  assert(charStringIsEmpty(NULL));
  return 0;
}

static int _testIsRegularStringNotEmpty(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  assertFalse(charStringIsEmpty(c));
  return 0;
}

static int _testCharStringEqualsSameString(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringCopyCString(c2, TEST_STRING);
  assert(charStringIsEqualTo(c, c2, false));
  return 0;
}

static int _testCharStringDoesEqualDifferentString(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringCopyCString(c2, OTHER_TEST_STRING);
  assertFalse(charStringIsEqualTo(c, c2, false));
  return 0;
}

static int _testCharStringEqualsSameStringInsensitive(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringCopyCString(c2, TEST_STRING_CAPS);
  assert(charStringIsEqualTo(c, c2, true));
  return 0;
}

static int _testCharStringDoesNotEqualSameStringInsensitive(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringCopyCString(c2, TEST_STRING_CAPS);
  assertFalse(charStringIsEqualTo(c, c2, false));
  return 0;
}

static int _testCharStringEqualsNull(void) {
  CharString c = newCharString();
  assertFalse(charStringIsEqualTo(c, NULL, false));
  return 0;
}

static int _testCharStringEqualsSameCString(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  assert(charStringIsEqualToCString(c, TEST_STRING, false));
  return 0;
}

static int _testCharStringNotEqualToDifferentCString(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  assertFalse(charStringIsEqualToCString(c, OTHER_TEST_STRING, false));
  return 0;
}

static int _testCharStringEqualsSameCStringInsensitive(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  assert(charStringIsEqualToCString(c, TEST_STRING_CAPS, true));
  return 0;
}

static int _testCharStringNotEqualsCStringInsensitive(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  assertFalse(charStringIsEqualToCString(c, TEST_STRING_CAPS, false));
  return 0;
}

static int _testCharStringEqualsCStringNull(void) {
  CharString c = newCharString();
  assertFalse(charStringIsEqualToCString(c, NULL, false));
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
