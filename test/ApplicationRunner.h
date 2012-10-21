#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include "TestRunner.h"
#include "LinkedList.h"
#include "PlatformUtilities.h"
#include "EventLogger.h"
#include "CharString.h"
#include "MrsWatson.h"

typedef struct {
  int currentIndex;
  char** outArray;
} ArgumentsCopyData;

typedef struct {
  char* applicationPath;
  char* resourcesPath;
  TestSuite results;
} TestEnvironmentMembers;
typedef TestEnvironmentMembers* TestEnvironment;

void runApplicationTest(const TestEnvironment testEnvironment,
  const char *testName, const char *testArguments,
  ReturnCodes expectedResultCode, boolByte analyzeOutput);
