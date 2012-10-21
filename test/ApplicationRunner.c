#include "PlatformUtilities.h"

#if UNIX
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif
#include "ApplicationRunner.h"
#include "TestRunner.h"
#include "CharString.h"
#include "AnalyzeFile.h"
#include "FileUtilities.h"

static char* _getTestInputFilename(const char* resourcesPath, const char* fileExtension) {
  CharString filename = newCharString();
  snprintf(filename->data, filename->capacity, "%s%caudio%ca440-stereo.%s",
    resourcesPath, PATH_DELIMITER, PATH_DELIMITER, fileExtension);
  return filename->data;
}

static char* _getTestOutputFilename(const char* testName, const char* fileExtension) {
  CharString filename = newCharString();
  char* space;
  char *spacePtr;
  snprintf(filename->data, filename->capacity, "out%c%s.%s", PATH_DELIMITER, testName, fileExtension);
  spacePtr = filename->data;
  do {
    space = strchr(spacePtr + 1, ' ');
    if(space == NULL || (unsigned int)(space - filename->data) > strlen(filename->data)) {
      break;
    }
    else {
      *space = '-';
    }
  } while(true);
  return filename->data;
}

static void _getDefaultArguments(TestEnvironment testEnvironment,
  const char *testName, CharString outString) {
  snprintf(outString->data, outString->capacity,
    "--log-file \"%s\" --input \"%s\" --output \"%s\"",
    _getTestOutputFilename(testName, "txt"),
    _getTestInputFilename(testEnvironment->resourcesPath, "pcm"),
    _getTestOutputFilename(testName, "pcm"));
}

static void _removeOutputFile(char* argument) {
  if(fileExists(argument)) {
    unlink(argument);
  }
}

static void _removeOutputFiles(const char* testName) {
  _removeOutputFile(_getTestOutputFilename(testName, "pcm"));
  _removeOutputFile(_getTestOutputFilename(testName, "txt"));
}

void runApplicationTest(const TestEnvironment testEnvironment,
  const char *testName, const char *testArguments,
  ReturnCodes expectedResultCode, boolByte anazyleOutput) {
  int resultCode = -1;
  CharString arguments = newCharString();
  CharString defaultArguments = newCharString();
  CharString failedAnalysisFunctionName = newCharString();
  unsigned long failedAnalysisSample;
  // TODO: Need to pass these back to the caller
  int testsPassed = 0;
  int testsFailed = 0;

  // Remove files from previous tests
  _removeOutputFiles(testName);

  // Create the command line argument
  appendCStringToCharString(arguments, testEnvironment->applicationPath);
  appendCStringToCharString(arguments, " ");
  _getDefaultArguments(testEnvironment, testName, defaultArguments);
  appendToCharString(arguments, defaultArguments);
  appendCStringToCharString(arguments, " ");
  appendCStringToCharString(arguments, testArguments);

  fprintf(stderr, "  %s: ", testName);
  fflush(stderr);

  // TODO: Move to FileUtilities
#if WINDOWS
  // TODO: mkdir();
#else
  mkdir("out", 0755);
#endif

#if WINDOWS
  logUnsupportedFeature("Application testing");
  return;
#else
  resultCode = system(arguments->data);
#endif

  if(resultCode == 255 || resultCode == -1 || resultCode == 127) {
    printTestFail();
    logCritical("Could not launch shell, got return code %d\n\
Please check the executable path specified in the --mrswatson-path argument.",
      resultCode);
    testsFailed++;
  }
  else if(WEXITSTATUS(resultCode) == expectedResultCode) {
    if(anazyleOutput) {
      if(analyzeFile(_getTestOutputFilename(testName, "pcm"), failedAnalysisFunctionName, &failedAnalysisSample)) {
        testsPassed++;
        _removeOutputFiles(testName);
        printTestSuccess();
      }
      else {
        printTestFail();
        printf("    in test '%s', while analyzing output for %s at sample %lu.\n",
          testName, failedAnalysisFunctionName->data, failedAnalysisSample);
        testsFailed++;
      }
    }
    else {
      testsPassed++;
      _removeOutputFiles(testName);
      printTestSuccess();
    }
  }
  else {
    printTestFail();
    printf("    in %s. Expected result code %d, got %d.\n", testName,
      expectedResultCode, WEXITSTATUS(resultCode));
    testsFailed++;
  }

  freeCharString(arguments);
  freeCharString(defaultArguments);
  freeCharString(failedAnalysisFunctionName);
}
