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

void runApplicationTest(const char *testName, LinkedList arguments, ReturnCodes expectedResultCode, AnalysisFuncPtr analysisFunction);
