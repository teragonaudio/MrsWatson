#ifndef MrsWatsonTest_TestRunner_h
#define MrsWatsonTest_TestRunner_h

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "base/PlatformUtilities.h"
#include "base/CharString.h"
#include "base/FileUtilities.h"

#if UNIX
#include <unistd.h>
#endif

#ifndef __func__
#define __func__ __FUNCTION__
#endif

typedef int (*TestCaseExecFunc)(void);
typedef void (*TestCaseSetupFunc)(void);
typedef void (*TestCaseTeardownFunc)(void);

typedef struct {
  char* name;
  char* filename;
  int lineNumber;
  TestCaseExecFunc testCaseFunc;
} TestCaseMembers;
typedef TestCaseMembers* TestCase;

typedef struct {
  char* name;
  int numSuccess;
  int numFail;
  int numSkips;
  LinkedList testCases;
  TestCaseSetupFunc setup;
  TestCaseTeardownFunc teardown;
  boolByte onlyPrintFailing;
} TestSuiteMembers;
typedef TestSuiteMembers* TestSuite;

void addTestToTestSuite(TestSuite testSuite, TestCase testCase);
void runTestSuite(void* testSuitePtr, void* extraData);
void runTestCase(void* item, void* extraData);
void printTestName(const char* testName);
void printTestSuccess(void);
void printTestFail(void);

TestSuite newTestSuite(char* name, TestCaseSetupFunc setup, TestCaseTeardownFunc teardown);
TestCase newTestCase(char* name, char* filename, int lineNumber, TestCaseExecFunc testCaseFunc);

#define addTest(testSuite, name, testCaseFunc) { \
  addTestToTestSuite(testSuite, newTestCase(name, __FILE__, __LINE__, testCaseFunc)); \
}

#define assert(condition) { \
  if(!(condition)) { \
    fprintf(stderr, "\nAssertion failed at %s:%d. ", getFileBasename(__FILE__), __LINE__); \
    return 1; \
  } \
}

#define assertFalse(condition) assert((condition) == false)
#define assertIsNull(condition) assert((condition) == NULL)
#define assertNotNull(condition) assert((condition) != NULL)

#define assertIntEquals(condition, expected) { \
  int result = condition; \
  if(result != expected) { \
    fprintf(stderr, "Assertion failed at %s:%d. Expected %d, got %d. ", getFileBasename(__FILE__), __LINE__, expected, result); \
    return 1; \
  } \
}

#define TEST_FLOAT_TOLERANCE 0.01
#define assertDoubleEquals(condition, expected, tolerance) { \
  double result = fabs(condition - expected); \
  if(result > tolerance) { \
    fprintf(stderr, "Assertion failed at %s:%d. Expected %g, got %g. ", getFileBasename(__FILE__), __LINE__, expected, condition); \
    return 1; \
  } \
}

#define assertCharStringEquals(result, expected) { \
  if(!isCharStringEqualToCString(result, expected, false)) { \
    fprintf(stderr, "Assertion failed at %s:%d. Expected '%s', got '%s'. ", getFileBasename(__FILE__), __LINE__, expected, result->data); \
    return 1; \
  } \
}

#define assertCharStringContains(result, match) { \
  if(strstr(result->data, match) == NULL) { \
    fprintf(stderr, "Assertion failed at %s:%d. Expected '%s' to contain '%s'. ", getFileBasename(__FILE__), __LINE__, result->data, match); \
    return 1; \
  } \
}

#endif
