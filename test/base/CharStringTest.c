#include "unit/TestRunner.h"
#include "base/CharString.h"

static char *const TEST_STRING = "test string";
static char *const TEST_STRING_CAPS = "TEST STRING";
static char *const OTHER_TEST_STRING = "other test string";

static int _testNewCharString(void) {
  CharString c = newCharString();
  assertIntEquals(c->length, kCharStringLengthDefault);
  assertCharStringEquals(c, "");
  freeCharString(c);
  return 0;
}

static int _testNewCharStringWithCapacity(void) {
  CharString c = newCharStringWithCapacity(123);
  assertIntEquals(c->length, 123);
  assertCharStringEquals(c, "");
  freeCharString(c);
  return 0;
}

static int _testClearCharString(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringClear(c);
  assertIntEquals(c->length, kCharStringLengthDefault);
  assertCharStringEquals(c, "");
  freeCharString(c);
  return 0;
}

static int _testCopyToCharString(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  assertIntEquals(c->length, kCharStringLengthDefault);
  assertCharStringEquals(c, TEST_STRING);
  freeCharString(c);
  return 0;
}

static int _testCopyCharStrings(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringCopy(c2, c);
  assertCharStringEquals(c, c2->data);
  freeCharString(c);
  freeCharString(c2);
  return 0;
}

static int _testIsEmptyStringEmpty(void) {
  CharString c = newCharString();
  assert(charStringIsEmpty(c));
  freeCharString(c);
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
  freeCharString(c);
  return 0;
}

static int _testAppendCharStrings(void) {
  CharString a = newCharStringWithCString("a");
  CharString b = newCharStringWithCString("b");
  charStringAppend(a, b);
  assertCharStringEquals(a, "ab");
  freeCharString(a);
  freeCharString(b);
  return 0;
}

static int _testAppendCharStringsOverCapacity(void) {
  CharString a = newCharStringWithCString("a");
  CharString b = newCharStringWithCString("1234567890");
  assertIntEquals(a->length, 2);
  charStringAppend(a, b);
  assertCharStringEquals(a, "a1234567890");
  assertIntEquals(a->length, 12);
  freeCharString(a);
  freeCharString(b);
  return 0;
}

static int _testCharStringEqualsSameString(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringCopyCString(c2, TEST_STRING);
  assert(charStringIsEqualTo(c, c2, false));
  freeCharString(c);
  freeCharString(c2);
  return 0;
}

static int _testCharStringDoesEqualDifferentString(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringCopyCString(c2, OTHER_TEST_STRING);
  assertFalse(charStringIsEqualTo(c, c2, false));
  freeCharString(c);
  freeCharString(c2);
  return 0;
}

static int _testCharStringEqualsSameStringInsensitive(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringCopyCString(c2, TEST_STRING_CAPS);
  assert(charStringIsEqualTo(c, c2, true));
  freeCharString(c);
  freeCharString(c2);
  return 0;
}

static int _testCharStringDoesNotEqualSameStringInsensitive(void) {
  CharString c, c2;
  c = newCharString();
  c2 = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringCopyCString(c2, TEST_STRING_CAPS);
  assertFalse(charStringIsEqualTo(c, c2, false));
  freeCharString(c);
  freeCharString(c2);
  return 0;
}

static int _testCharStringEqualsNull(void) {
  CharString c = newCharString();
  assertFalse(charStringIsEqualTo(c, NULL, false));
  freeCharString(c);
  return 0;
}

static int _testCharStringEqualsSameCString(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  assert(charStringIsEqualToCString(c, TEST_STRING, false));
  freeCharString(c);
  return 0;
}

static int _testCharStringNotEqualToDifferentCString(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  assertFalse(charStringIsEqualToCString(c, OTHER_TEST_STRING, false));
  freeCharString(c);
  return 0;
}

static int _testCharStringEqualsSameCStringInsensitive(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  assert(charStringIsEqualToCString(c, TEST_STRING_CAPS, true));
  freeCharString(c);
  return 0;
}

static int _testCharStringNotEqualsCStringInsensitive(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  assertFalse(charStringIsEqualToCString(c, TEST_STRING_CAPS, false));
  freeCharString(c);
  return 0;
}

static int _testCharStringEqualsCStringNull(void) {
  CharString c = newCharString();
  assertFalse(charStringIsEqualToCString(c, NULL, false));
  freeCharString(c);
  return 0;
}

static int _testFreeNullCharString(void) {
  freeCharString(NULL);
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

  addTest(testSuite, "AppendCharStrings", _testAppendCharStrings);
  addTest(testSuite, "AppendCharStringsOverCapacity", _testAppendCharStringsOverCapacity);

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

  addTest(testSuite, "FreeNullCharString", _testFreeNullCharString);

  return testSuite;
}
