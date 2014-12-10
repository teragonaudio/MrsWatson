#ifndef MrsWatson_ApplicationRunner_h
#define MrsWatson_ApplicationRunner_h

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include "unit/TestRunner.h"
#include "base/LinkedList.h"
#include "logging/EventLogger.h"
#include "base/CharString.h"
#include "MrsWatson.h"

extern const char *kDefaultTestOutputFileType;

typedef struct {
    int currentIndex;
    char **outArray;
} ArgumentsCopyData;

typedef struct {
    char *applicationPath;
    char *resourcesPath;
    TestSuite results;
} TestEnvironmentMembers;
typedef TestEnvironmentMembers *TestEnvironment;
TestEnvironment newTestEnvironment(char *applicationPath, char *resourcesPath);

void runApplicationTest(const TestEnvironment testEnvironment,
                        const char *testName, CharString testArguments,
                        ReturnCodes expectedResultCode, const char *outputFileType);

CharString buildTestArgumentString(const char *arguments, ...);
CharString getTestResourceFilename(const char *resourcesPath, const char *resourceType, const char *resourceName);
CharString getTestOutputFilename(const char *testName, const char *fileExtension);

void freeTestEnvironment(TestEnvironment testEnvironment);

#endif
