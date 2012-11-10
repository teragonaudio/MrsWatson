#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include "TestRunner.h"
#include "base/LinkedList.h"
#include "base/PlatformUtilities.h"
#include "logging/EventLogger.h"
#include "base/CharString.h"
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

TestEnvironment newTestEnvironment(char *applicationPath, char *resourcesPath);

void runApplicationTest(const TestEnvironment testEnvironment,
  const char *testName, const char *testArguments,
  ReturnCodes expectedResultCode, boolByte analyzeOutput);

void freeTestEnvironment(TestEnvironment testEnvironment);
