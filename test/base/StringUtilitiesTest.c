#include "unit/TestRunner.h"
#include "base/StringUtilities.h"

// This function is not technically public, but we test against it instead of
// the public version in order to set a shorter line length. This makes test
// cases much easier to construct.
extern boolByte _wrapString(const char* srcString, char* destString,
  int indentSize, int lineLength);

static int _testIsLetter(void) {
  assert(isLetter('a'));
  assert(isLetter('A'));
  return 0;
}

static int _testIsNotLetter(void) {
  assertFalse(isLetter('0'));
  return 0;
}

static int _testIsNumber(void) {
  assert(isNumber('0'));
  return 0;
}

static int _testIsNotNumber(void) {
  assertFalse(isNumber('a'));
  return 0;
}

static int _testWrapNullSourceString(void) {
  assertFalse(wrapString(NULL, "test", 0));
  return 0;
}

static int _testWrapNullDestString(void) {
  assertFalse(wrapString("test", NULL, 0));
  return 0;
}

static int _testWrapInvalidIndentSize(void) {
  assertFalse(wrapString("test", "test", -1));
  return 0;
}

static int _testWrapString(void) {
  char *src = "1234 6789 bcde 01";
  CharString dest = newCharString();
  assert(_wrapString(src, dest->data, 0, 0xf));
  assertCharStringEquals(dest, "1234 6789 bcde\n01");
  return 0;
}

static int _testWrapStringWithIndent(void) {
  char *src = "1234 6789 bcde 01";
  CharString dest = newCharString();
  assert(_wrapString(src, dest->data, 1, 0xe));
  assertCharStringEquals(dest, " 1234 6789\n bcde 01");
  return 0;
}

static int _testWrapStringLongerThanLine(void) {
  char *src = "123456789abcdef12";
  CharString dest = newCharString();
  assert(_wrapString(src, dest->data, 0, 0xf));
  assertCharStringEquals(dest, "123456789abcde-\nf12");
  return 0;
}

static int _testConvertIntIdToString(void) {
  CharString test = newCharStringWithCString("abcd");
  unsigned long id = convertStringIdToInt(test);
  assertIntEquals(id, 0x61626364);
  return 0;
}

static int _testConvertZeroIntIdToString(void) {
  CharString c = convertIntIdToString(0);
  assert(charStringIsEmpty(c));
  return 0;
}

static int _testConvertStringIdToInt(void) {
  CharString c = convertIntIdToString(0x61626364);
  assertCharStringEquals(c, "abcd");
  return 0;
}

static int _testConvertEmptyStringIdToInt(void) {
  unsigned long id = convertStringIdToInt(newCharString());
  assertIntEquals(id, 0);
  return 0;
}

static int _testConvertNullStringIdToInt(void) {
  unsigned long id = convertStringIdToInt(NULL);
  assertIntEquals(id, 0);
  return 0;
}

static int _testConvertInvalidStringIdToInt(void) {
  unsigned long id = convertStringIdToInt(newCharStringWithCString("a"));
  assertIntEquals(id, 0);
  return 0;
}

TestSuite addStringUtilitiesTests(void);
TestSuite addStringUtilitiesTests(void) {
  TestSuite testSuite = newTestSuite("StringUtilities", NULL, NULL);
  addTest(testSuite, "IsLetter", _testIsLetter);
  addTest(testSuite, "IsNotLetter", _testIsNotLetter);
  addTest(testSuite, "IsNumber", _testIsNumber);
  addTest(testSuite, "IsNotNumber", _testIsNotNumber);

  addTest(testSuite, "ConvertIntIdToString", _testConvertIntIdToString);
  addTest(testSuite, "ConvertZeroIntIdToString", _testConvertZeroIntIdToString);
  addTest(testSuite, "ConvertStringIdToInt", _testConvertStringIdToInt);
  addTest(testSuite, "ConvertNullStringIdToInt", _testConvertNullStringIdToInt);
  addTest(testSuite, "ConvertEmptyStringIdToInt", _testConvertEmptyStringIdToInt);
  addTest(testSuite, "ConvertInvalidStringIdToInt", _testConvertInvalidStringIdToInt);

  addTest(testSuite, "WrapNullSourceString", _testWrapNullSourceString);
  addTest(testSuite, "WrapNullDestString", _testWrapNullDestString);
  addTest(testSuite, "InvalidIndentSize", _testWrapInvalidIndentSize);
  addTest(testSuite, "WrapString", _testWrapString);
  addTest(testSuite, "WrapStringWithIndent", _testWrapStringWithIndent);
  addTest(testSuite, "WrapStringLongerThanLine", _testWrapStringLongerThanLine);
  return testSuite;
}
