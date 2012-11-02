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

TestEnvironment newTestEnvironment(char *applicationPath, char *resourcesPath) {
  TestEnvironment testEnvironment = (TestEnvironment)malloc(sizeof(TestEnvironmentMembers));
  testEnvironment->applicationPath = applicationPath;
  testEnvironment->resourcesPath = resourcesPath;
  testEnvironment->results = newTestSuite("Results", NULL, NULL);
  return testEnvironment;
}

void freeTestEnvironment(TestEnvironment testEnvironment) {
  free(testEnvironment->results);
  free(testEnvironment);
}

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

static char* _getTestPluginResourcesPath(const char* resourcesPath) {
  CharString pluginRoot = newCharString();
  snprintf(pluginRoot->data, pluginRoot->capacity, "%s%cvst%c%s",
    resourcesPath, PATH_DELIMITER, PATH_DELIMITER, getShortPlatformName());
  return pluginRoot->data;
}

static void _getDefaultArguments(TestEnvironment testEnvironment,
  const char *testName, CharString outString) {
  snprintf(outString->data, outString->capacity,
    "--log-file \"%s\" --verbose --input \"%s\" --output \"%s\" --plugin-root \"%s\"",
    _getTestOutputFilename(testName, "txt"),
    _getTestInputFilename(testEnvironment->resourcesPath, "pcm"),
    _getTestOutputFilename(testName, "pcm"),
    _getTestPluginResourcesPath(testEnvironment->resourcesPath));
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
  ReturnCodes resultCode = -1;
  CharString arguments = newCharString();
  CharString defaultArguments = newCharString();
  CharString failedAnalysisFunctionName = newCharString();
  unsigned long failedAnalysisSample;

  // Remove files from a previous test run
  _removeOutputFiles(testName);

  // Create the command line argument
  appendCStringToCharString(arguments, testEnvironment->applicationPath);
  appendCStringToCharString(arguments, " ");
  _getDefaultArguments(testEnvironment, testName, defaultArguments);
  appendToCharString(arguments, defaultArguments);
  appendCStringToCharString(arguments, " ");
  appendCStringToCharString(arguments, testArguments);

  if(!testEnvironment->results->onlyPrintFailing) {
    printTestName(testName);
  }

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
  resultCode = (ReturnCodes)WEXITSTATUS(system(arguments->data));
#endif

  if(resultCode == 255 || (int)resultCode == -1 || resultCode == 127) {
    if(testEnvironment->results->onlyPrintFailing) {
      printTestName(testName);
    }
    printTestFail();
    logCritical("Could not launch shell, got return code %d\n\
Please check the executable path specified in the --mrswatson-path argument.",
      resultCode);
    testEnvironment->results->numFail++;
  }
  else if(resultCode == expectedResultCode) {
    if(anazyleOutput) {
      if(analyzeFile(_getTestOutputFilename(testName, "pcm"), failedAnalysisFunctionName, &failedAnalysisSample)) {
        testEnvironment->results->numSuccess++;
        _removeOutputFiles(testName);
        if(!testEnvironment->results->onlyPrintFailing) {
          printTestSuccess();
        }
      }
      else {
        if(testEnvironment->results->onlyPrintFailing) {
          printTestName(testName);
        }
        fprintf(stderr, "Analysis function %s failed at sample %lu. ",
          failedAnalysisFunctionName->data, failedAnalysisSample);
        printTestFail();
        testEnvironment->results->numFail++;
      }
    }
    else {
      testEnvironment->results->numSuccess++;
      _removeOutputFiles(testName);
      if(!testEnvironment->results->onlyPrintFailing) {
        printTestSuccess();
      }
    }
  }
  else {
    if(testEnvironment->results->onlyPrintFailing) {
      printTestName(testName);
    }
    fprintf(stderr, "Expected result code %d, got %d. ", expectedResultCode, resultCode);
    printTestFail();
    testEnvironment->results->numFail++;
  }

  freeCharString(arguments);
  freeCharString(defaultArguments);
  freeCharString(failedAnalysisFunctionName);
}
