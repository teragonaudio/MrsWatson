#include "unit/TestRunner.h"
#include "base/CharString.h"
#include "base/LinkedList.h"

static char *const TEST_STRING = "test string";
static char *const TEST_STRING_CAPS = "TEST STRING";
static char *const OTHER_TEST_STRING = "other test string";

static int _testNewCharString(void) {
  CharString c = newCharString();
  assertSizeEquals(c->capacity, kCharStringLengthDefault);
  assertCharStringEquals(c, "");
  freeCharString(c);
  return 0;
}

static int _testNewCharStringWithCapacity(void) {
  size_t testSize = 123;
  CharString c = newCharStringWithCapacity(testSize);
  assertSizeEquals(c->capacity, testSize);
  assertCharStringEquals(c, "");
  freeCharString(c);
  return 0;
}

static int _testNewObjectWithNullCString(void) {
  CharString c = newCharStringWithCString(NULL);
  assertSizeEquals(c->capacity, kCharStringLengthDefault);
  assertCharStringEquals(c, "");
  freeCharString(c);
  return 0;
}

static int _testNewObjectWithEmptyCString(void) {
  CharString c = newCharStringWithCString(EMPTY_STRING);
  assertSizeEquals(c->capacity, kCharStringLengthDefault);
  assertCharStringEquals(c, EMPTY_STRING);
  freeCharString(c);
  return 0;
}

static int _testClearCharString(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  charStringClear(c);
  assertSizeEquals(c->capacity, kCharStringLengthDefault);
  assertCharStringEquals(c, "");
  freeCharString(c);
  return 0;
}

static int _testCopyToCharString(void) {
  CharString c = newCharString();
  charStringCopyCString(c, TEST_STRING);
  assertSizeEquals(c->capacity, kCharStringLengthDefault);
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
  assertSizeEquals(a->capacity, (size_t)2);
  charStringAppend(a, b);
  assertCharStringEquals(a, "a1234567890");
  assertSizeEquals(a->capacity, (size_t)12);
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

static int _testIsLetter(void) {
  CharString c = newCharStringWithCString("a");
  assert(charStringIsLetter(c, 0));
  freeCharString(c);
  return 0;
}

static int _testIsNotLetter(void) {
  CharString c = newCharStringWithCString("0");
  assertFalse(charStringIsLetter(c, 0));
  freeCharString(c);
  return 0;
}

static int _testIsNumber(void) {
  CharString c = newCharStringWithCString("0");
  assert(charStringIsNumber(c, 0));
  freeCharString(c);
  return 0;
}

static int _testIsNotNumber(void) {
  CharString c = newCharStringWithCString("a");
  assertFalse(charStringIsNumber(c, 0));
  freeCharString(c);
  return 0;
}

static int _testSplitString(void) {
  CharString c = newCharStringWithCString("abc,def,ghi,");
  LinkedList l = charStringSplit(c, ',');
  CharString* items = NULL;

  assertNotNull(l);
  assertIntEquals(linkedListLength(l), 3);
  items = (CharString*)linkedListToArray(l);
  assertNotNull(items);
  assertCharStringEquals(items[0], "abc");
  assertCharStringEquals(items[1], "def");
  assertCharStringEquals(items[2], "ghi");

  freeLinkedListAndItems(l, (LinkedListFreeItemFunc)freeCharString);
  freeCharString(c);
  free(items);
  return 0;
}

static int _testSplitStringWithoutDelimiter(void) {
  const char* expected = "abcdefg";
  CharString c = newCharStringWithCString(expected);
  CharString c2 = NULL;
  LinkedList l = charStringSplit(c, ',');

  assertNotNull(l);
  assertIntEquals(linkedListLength(l), 1);
  c2 = l->item;
  assertCharStringEquals(c2, expected);

  freeLinkedListAndItems(l, (LinkedListFreeItemFunc)freeCharString);
  freeCharString(c);
  return 0;
}

static int _testSplitStringNULLDelimiter(void) {
  CharString c = newCharStringWithCString("abcdefh");
  LinkedList l = charStringSplit(c, 0);
  assertIsNull(l);
  freeCharString(c);
  return 0;
}

static int _testSplitStringEmptyString(void) {
  CharString c = newCharString();
  LinkedList l = charStringSplit(c, ',');
  assertNotNull(l);
  assertIntEquals(linkedListLength(l), 0);
  freeCharString(c);
  freeLinkedList(l);
  return 0;
}

// This function is not technically public, but we test against it instead of
// the public version in order to set a shorter line length. This makes test
// cases much easier to construct.
extern boolByte _charStringWrap(const char* srcString, char* destString,
  int indentSize, int lineLength);

static int _testWrapNullSourceString(void) {
  assertFalse(charStringWrap(NULL, 0));
  return 0;
}

static int _testWrapString(void) {
  CharString src = newCharStringWithCString("1234 6789 bcde 01");
  // Create dest string the same way as in wrapString(), cheap I know...
  CharString dest = newCharStringWithCapacity(src->capacity * 2);
  _charStringWrap(src->data, dest->data, 0, 0x10);
  assertCharStringEquals(dest, "1234 6789 bcde\n01");
  freeCharString(src);
  freeCharString(dest);
  return 0;
}

static int _testWrapStringWithIndent(void) {
  CharString src = newCharStringWithCString("1234 6789 bcde 01");
  // Create dest string the same way as in wrapString(), cheap I know...
  CharString dest = newCharStringWithCapacity(src->capacity * 2);
  _charStringWrap(src->data, dest->data, 1, 0xe);
  assertCharStringEquals(dest, " 1234 6789\n bcde 01");
  freeCharString(src);
  freeCharString(dest);
  return 0;
}

static int _testWrapStringLongerThanLine(void) {
  CharString src = newCharStringWithCString("123456789abcdef12");
  // Create dest string the same way as in wrapString(), cheap I know...
  CharString dest = newCharStringWithCapacity(src->capacity * 2);
  _charStringWrap(src->data, dest->data, 0, 0xf);
  assertCharStringEquals(dest, "123456789abcde-\nf12");
  freeCharString(src);
  freeCharString(dest);
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
  addTest(testSuite, "NewObjectWithNullCString", _testNewObjectWithNullCString);
  addTest(testSuite, "NewObjectWithEmptyCString", _testNewObjectWithEmptyCString);

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

  addTest(testSuite, "IsLetter", _testIsLetter);
  addTest(testSuite, "IsNotLetter", _testIsNotLetter);
  addTest(testSuite, "IsNumber", _testIsNumber);
  addTest(testSuite, "IsNotNumber", _testIsNotNumber);

  addTest(testSuite, "SplitString", _testSplitString);
  addTest(testSuite, "SplitStringWithoutDelimiter", _testSplitStringWithoutDelimiter);
  addTest(testSuite, "SplitStringNULLDelimiter", _testSplitStringNULLDelimiter);
  addTest(testSuite, "SplitStringEmptyString", _testSplitStringEmptyString);

  addTest(testSuite, "WrapNullSourceString", _testWrapNullSourceString);
  addTest(testSuite, "WrapString", _testWrapString);
  addTest(testSuite, "WrapStringWithIndent", _testWrapStringWithIndent);
  addTest(testSuite, "WrapStringLongerThanLine", _testWrapStringLongerThanLine);

  addTest(testSuite, "FreeNullCharString", _testFreeNullCharString);

  return testSuite;
}
