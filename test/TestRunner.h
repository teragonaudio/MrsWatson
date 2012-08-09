#include <stdio.h>
#include <string.h>
#include "CharString.h"

#define _assert(condition) \
  { \
    if(!(condition)) { \
      printf("FAIL at %s(), line %d\n", __func__, __LINE__); \
      return 1; \
    } \
  }

#define _assertFalse(condition) _assert((condition) == false)

#define _assertIntEquals(condition, expected) \
  { \
    int result = condition; \
    if(result != expected) { \
      printf("FAIL at %s(), line %d. Expected %d, got %d.\n", __func__, __LINE__, expected, result); \
      return 1; \
    } \
  }

#define _assertDoubleEquals(condition, expected) \
  { \
    double result = condition; \
    if(result != expected) { \
      printf("FAIL at %s(), line %d. Expected %g, got %g.\n", __func__, __LINE__, expected, result); \
      return 1; \
    } \
  }

#define _assertCharStringEquals(result, expected) \
  { \
    if(!isCharStringEqualToCString(result, expected, false)) { \
      printf("FAIL at %s(), line %d. Expected %s, got %s.\n", __func__, __LINE__, expected, result->data); \
      return 1; \
    } \
  }

#define _runTest(testName, test, setup, teardown) \
  { \
    printTestStarted(testName); \
    setup(); \
    int result = test(); \
    if(!result) { \
      testsPassed++; \
      printTestSuccess(); \
    } else { \
      testsFailed++; \
    } \
    teardown(); \
  }

#if WINDOWS
#define FILE_BASENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define FILE_BASENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define _startTestSection() \
  { \
    printf("Running tests in %s\n", FILE_BASENAME); \
  }

extern int testsPassed;
extern int testsFailed;

static void emptySetup(void) { }
static void emptyTeardown(void) { }

static void printTestStarted(const char* testName) {
  printf("  %s: ", testName);
}

static void printTestSuccess(void) {
  printf("OK\n");
}
