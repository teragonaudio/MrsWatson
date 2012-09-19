#include <stdlib.h>
#include "TestRunner.h"

// In both the TestSuite and TestCase objects we assume that we do not need ownership of
// the strings passed in, since they should be allocated on the heap and live for the
// lifetime of the program.
TestSuite newTestSuite(char* name) {
  TestSuite testSuite = malloc(sizeof(TestSuite));
  testSuite->name = name;
  testSuite->testCases = newLinkedList();
  return testSuite;
}

TestCase newTestCase(char* name, char* filename, int lineNumber, TestCaseExecFunc testCaseFunc) {
  TestCase testCase = malloc(sizeof(TestCase));
  testCase->name = name;
  testCase->filename = filename;
  testCase->lineNumber = lineNumber;
  testCase->testCaseFunc = testCaseFunc;
  return testCase;
}

void addTestToTestSuite(TestSuite testSuite, TestCase testCase) {
  appendItemToList(testSuite->testCases, testCase);
}

static void _runTestCase(void* item, void* extraData) {
  TestCase testCase = (TestCase)item;
  printf("  %s: ", testCase->name);
  testCase->result = testCase->testCaseFunc(testCase);
}

void executeTestSuite(TestSuite testSuite) {
  printf("Running tests in %s\n", testSuite->name);
  foreachItemInList(testSuite->testCases, _runTestCase, NULL);
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

static void _printTestCase(void* item, void* extraData) {
  TestCase testCase = (TestCase)item;
  printf("  %s: ", testCase->name);
  if(testCase->result) {
    printTestSuccess();
  }
  else {
    // TODO: Print out source file, line number
    printTestFail();
  }
}

void printTestSuiteResult(TestSuite testSuite) {
  foreachItemInList(testSuite->testCases, _printTestCase, NULL);
}
