#include <unistd.h>
#include <stdlib.h>
#include "TestRunner.h"
#include "LinkedList.h"
#include "PlatformUtilities.h"
#include "EventLogger.h"
#include "CharString.h"
#include "MrsWatson.h"

typedef boolByte (*AnalysisFuncPtr)(const char* filename);

typedef struct {
  int currentIndex;
  char** outArray;
} ArgumentsCopyData;

LinkedList getDefaultArguments(const char *testName);
void runApplicationTest(char *applicationPath, const char *testName, LinkedList testArguments, ReturnCodes expectedResultCode, AnalysisFuncPtr analysisFunction);
