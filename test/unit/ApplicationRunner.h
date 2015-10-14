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

typedef enum {
    kTestOutputNone,
    kTestOutputAiff,
    kTestOutputFlac,
    kTestOutputPcm,
    kTestOutputText,
    kTestOutputWave,
} TestOutputType;

typedef struct {
    int currentIndex;
    char **outArray;
} ArgumentsCopyData;

int runIntegrationTest(const char *testName,
                       CharString testArguments,
                       ReturnCode expectedResultCode,
                       const TestOutputType testOutputType,
                       const CharString mrsWatsonExePath,
                       const CharString resourcesPath);

CharString buildTestArgumentString(const char *arguments, ...);
CharString getTestResourcePath(const CharString resourcesPath, const char *resourceType, const char *resourceName);
CharString getTestOutputFilename(const char *testName, const TestOutputType testOutputType);

#endif
