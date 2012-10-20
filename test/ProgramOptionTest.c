#include "TestRunner.h"
#include "ProgramOption.h"

static int _testNewProgramOptions(void) {
  return 0;
}

static int _testAddNewProgramOption(void) {
  return 0;
}

static int _testParseCommandLineShortOption(void) {
  return 0;
}

static int _testParseCommandLineLongOption(void) {
  return 0;
}

static int _testParseCommandLineWrongType(void) {
  return 0;
}

static int _testParseCommandLineRequiredOption(void) {
  return 0;
}

static int _testFindProgramOptionFromString(void) {
  return 0;
}

static int _testFindProgramOptionFromStringInvalid(void) {
  return 0;
}

TestSuite addProgramOptionTests(void);
TestSuite addProgramOptionTests(void) {
  TestSuite testSuite = newTestSuite("ProgramOption", NULL, NULL);
  addTest(testSuite, "NewObject", NULL); // _testNewProgramOptions);
  addTest(testSuite, "AddNewProgramOption", NULL); // _testAddNewProgramOption);
  addTest(testSuite, "ParseCommandLineShortOption", NULL); // _testParseCommandLineShortOption);
  addTest(testSuite, "ParseCommandLineLongOption", NULL); // _testParseCommandLineLongOption);
  addTest(testSuite, "ParseCommandLineWrongType", NULL); // _testParseCommandLineWrongType);
  addTest(testSuite, "ParseCommandLineRequiredOption", NULL); // _testParseCommandLineRequiredOption);
  addTest(testSuite, "FindProgramOptionFromString", NULL); // _testFindProgramOptionFromString);
  addTest(testSuite, "FindProgramOptionFromStringInvalid", NULL); // _testFindProgramOptionFromStringInvalid);
  return testSuite;
}
