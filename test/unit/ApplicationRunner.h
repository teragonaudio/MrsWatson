#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include "unit/TestRunner.h"
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
  const char *testName, CharString testArguments,
  ReturnCodes expectedResultCode, boolByte analyzeOutput);

CharString buildTestArgumentString(const char* arguments, ...);
char* getTestFilename(const char* resourcesPath, const char* resourceType, const char* resourceName);

void freeTestEnvironment(TestEnvironment testEnvironment);
