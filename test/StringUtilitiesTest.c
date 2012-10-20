#include "TestRunner.h"
#include "StringUtilities.h"

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
  return 0;
}

static int _testWrapNullDestString(void) {
  return 0;
}

static int _testWrapString(void) {
  return 0;
}

static int _testWrapStringWithIndent(void) {
  return 0;
}

static int _testWrapStringLongerThanLine(void) {
  return 0;
}

TestSuite addStringUtilitiesTests(void);
TestSuite addStringUtilitiesTests(void) {
  TestSuite testSuite = newTestSuite("StringUtilities", NULL, NULL);
  addTest(testSuite, "IsLetter", _testIsLetter);
  addTest(testSuite, "IsNotLetter", _testIsNotLetter);
  addTest(testSuite, "IsNumber", _testIsNumber);
  addTest(testSuite, "IsNotNumber", _testIsNotNumber);
  addTest(testSuite, "WrapNullSourceString", NULL); // _testWrapNullSourceString);
  addTest(testSuite, "WrapNullDestString", NULL); // _testWrapNullDestString);
  addTest(testSuite, "WrapString", NULL); // _testWrapString);
  addTest(testSuite, "WrapStringWithIndent", NULL); // _testWrapStringWithIndent);
  addTest(testSuite, "WrapStringLongerThanLine", NULL); // _testWrapStringLongerThanLine);
  return testSuite;
}
