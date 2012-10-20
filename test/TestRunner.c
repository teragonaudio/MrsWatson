#include <stdlib.h>
#if WINDOWS
#include <io.h>
#endif

#include "TestRunner.h"

// In both the TestSuite and TestCase objects we assume that we do not need ownership of
// the strings passed in, since they should be allocated on the heap and live for the
// lifetime of the program.
TestSuite newTestSuite(char* name, TestCaseSetupFunc setup, TestCaseTeardownFunc teardown) {
  TestSuite testSuite = malloc(sizeof(TestSuiteMembers));
  testSuite->name = name;
  testSuite->numSuccess = 0;
  testSuite->numFail = 0;
  testSuite->numSkips = 0;
  testSuite->testCases = newLinkedList();
  testSuite->setup = setup;
  testSuite->teardown = teardown;
  return testSuite;
}

TestCase newTestCase(char* name, char* filename, int lineNumber, TestCaseExecFunc testCaseFunc) {
  TestCase testCase = malloc(sizeof(TestCaseMembers));
  testCase->name = name;
  testCase->filename = filename;
  testCase->lineNumber = lineNumber;
  testCase->testCaseFunc = testCaseFunc;
  return testCase;
}

void addTestToTestSuite(TestSuite testSuite, TestCase testCase) {
  appendItemToList(testSuite->testCases, testCase);
}

void printTestSuccess(void) {
  if(isatty(2)) {
    printf("\x1b%sOK\x1b%s\n", ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
  }
  else {
    printf("OK\n");
  }
}

void printTestFail(void) {
  if(isatty(2)) {
    printf("\x1b%sFAIL\x1b%s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
  }
  else {
    printf("FAIL\n");
  }
}

static void _printTestSkipped(void) {
  if(isatty(2)) {
    printf("\x1b%sSkipped\x1b%s\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
  }
  else {
    printf("Skipped\n");
  }
}

void runTestCase(void* item, void* extraData) {
  TestCase testCase = (TestCase)item;
  TestSuite testSuite = (TestSuite)extraData;
  int result;
  printf("  %s: ", testCase->name);
  // Flush standard output in case the test crashes. That way at least the
  // crashing test name is seen.
  fflush(stdout);

  if(testCase->testCaseFunc != NULL) {
    if(testSuite->setup != NULL) {
      testSuite->setup();
    }
    result = testCase->testCaseFunc();
    if(result == 0) {
      printTestSuccess();
      testSuite->numSuccess++;
    }
    else {
      testSuite->numFail++;
    }

    if(testSuite->teardown != NULL) {
      testSuite->teardown();
    }
  }
  else {
    _printTestSkipped();
    testSuite->numSkips++;
  }
}

void runTestSuite(void* testSuitePtr, void* extraData) {
  TestSuite testSuite = (TestSuite)testSuitePtr;
  printf("Running tests in %s:\n", testSuite->name);
  foreachItemInList(testSuite->testCases, runTestCase, testSuite);
}
