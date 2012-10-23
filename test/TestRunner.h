#ifndef MrsWatsonTest_TestRunner_h
#define MrsWatsonTest_TestRunner_h

#include <stdio.h>
#include <string.h>
#if ! WINDOWS
#include <unistd.h>
#endif
#include "PlatformUtilities.h"
#include "CharString.h"
#include "FileUtilities.h"

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
} TestSuiteMembers;
typedef TestSuiteMembers* TestSuite;

void addTestToTestSuite(TestSuite testSuite, TestCase testCase);
void runTestSuite(void* testSuitePtr, void* extraData);
void runTestCase(void* item, void* extraData);
void printTestSuccess(void);
void printTestFail(void);

TestSuite newTestSuite(char* name, TestCaseSetupFunc setup, TestCaseTeardownFunc teardown);
TestCase newTestCase(char* name, char* filename, int lineNumber, TestCaseExecFunc testCaseFunc);

#define addTest(testSuite, name, testCaseFunc) { \
  addTestToTestSuite(testSuite, newTestCase(name, __FILE__, __LINE__, testCaseFunc)); \
}

#define assert(condition) { \
  if(!(condition)) { \
    printTestFail(); \
    printf("    at %s:%d\n", getFileBasename(__FILE__), __LINE__); \
    return 1; \
  } \
}

#define assertFalse(condition) assert((condition) == false)
#define assertIsNull(condition) assert((condition) == NULL)
#define assertNotNull(condition) assert((condition) != NULL)

#define assertIntEquals(condition, expected) { \
  int result = condition; \
  if(result != expected) { \
    printTestFail(); \
    printf("    at %s:%d. Expected %d, got %d.\n", getFileBasename(__FILE__), __LINE__, expected, result); \
    return 1; \
  } \
}

#define TEST_FLOAT_TOLERANCE 0.01
#define assertDoubleEquals(condition, expected) { \
  double result = fabs(condition - expected); \
  if(result > TEST_FLOAT_TOLERANCE) { \
    printTestFail(); \
    printf("    at %s:%d. Expected %g, got %g.\n", getFileBasename(__FILE__), __LINE__, expected, condition); \
    return 1; \
  } \
}

#define assertCharStringEquals(result, expected) { \
  if(!isCharStringEqualToCString(result, expected, false)) { \
    printTestFail(); \
    printf("    at %s:%d. Expected %s, got %s.\n", getFileBasename(__FILE__), __LINE__, expected, result->data); \
    return 1; \
  } \
}

#endif
