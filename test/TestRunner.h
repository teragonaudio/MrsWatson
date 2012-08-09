#include <stdio.h>
#include <string.h>

#define _assert(condition) \
  { \
    if(!(condition)) { \
      printTestFailed(__func__, __LINE__); \
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

static void printTestStarted(const char* testName) {
  printf("  %s: ", testName);
}

static void printTestSuccess(void) {
  printf("OK\n");
}

static void printTestFailed(const char* functionName, const int lineNumber) {
  printf("FAIL at %s(), line %d\n", functionName, lineNumber);
}
