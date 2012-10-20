#include "TestRunner.h"
#include "StringUtilities.h"

static int _testIsLetter(void) {
  return 0;
}

static int _testIsNotLetter(void) {
  return 0;
}

static int _testIsNumber(void) {
  return 0;
}

static int _testIsNotNumber(void) {
  return 0;
}

static int _testWrapNullSourceString(void) {
  return 0;
}

static int _testWrapNullDestString(void) {
  return 0;
}

static int _testWrappString(void) {
  return 0;
}

static int _testWrappStringWithIndent(void) {
  return 0;
}

static int _testWrappStringLongerThanLine(void) {
  return 0;
}

TestSuite addStringUtilitiesTests(void);
TestSuite addStringUtilitiesTests(void) {
  TestSuite testSuite = newTestSuite("StringUtilities", NULL, NULL);
  addTest(testSuite, "IsLetter", NULL); // _testIsLetter);
  addTest(testSuite, "IsNotLetter", NULL); // _testIsNotLetter);
  addTest(testSuite, "IsNumber", NULL); // _testIsNumber);
  addTest(testSuite, "IsNotNumber", NULL); // _testIsNotNumber);
  addTest(testSuite, "WrapNullSourceString", NULL); // _testWrapNullSourceString);
  addTest(testSuite, "WrapNullDestString", NULL); // _testWrapNullDestString);
  addTest(testSuite, "WrappString", NULL); // _testWrappString);
  addTest(testSuite, "WrappStringWithIndent", NULL); // _testWrappStringWithIndent);
  addTest(testSuite, "WrappStringLongerThanLine", NULL); // _testWrappStringLongerThanLine);
  return testSuite;
}
