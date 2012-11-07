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
  snprintf(filename->data, filename->length, "%s%caudio%ca440-stereo.%s",
    resourcesPath, PATH_DELIMITER, PATH_DELIMITER, fileExtension);
  return filename->data;
}

static char* _getTestOutputFilename(const char* testName, const char* fileExtension) {
  CharString filename = newCharString();
  char* space;
  char *spacePtr;
  snprintf(filename->data, filename->length, "out%c%s.%s", PATH_DELIMITER, testName, fileExtension);
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
  snprintf(pluginRoot->data, pluginRoot->length, "%s%cvst%c%s",
    resourcesPath, PATH_DELIMITER, PATH_DELIMITER, getShortPlatformName());
  return pluginRoot->data;
}

static void _getDefaultArguments(TestEnvironment testEnvironment,
  const char *testName, CharString outString) {
  snprintf(outString->data, outString->length,
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
  ReturnCodes expectedResultCode, boolByte anazyleOutput)
{
  int result = -1;
  ReturnCodes resultCode = (ReturnCodes)result;
  CharString arguments = newCharStringWithCapacity(STRING_LENGTH_LONG);
  CharString defaultArguments = newCharStringWithCapacity(STRING_LENGTH_LONG);
  CharString failedAnalysisFunctionName = newCharString();
  unsigned long failedAnalysisSample;

  // Remove files from a previous test run
  _removeOutputFiles(testName);
  makeDirectory(newCharStringWithCString("out"));

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

#if WINDOWS
  logUnsupportedFeature("Application testing");
  return;
#else
  result = system(arguments->data);
  resultCode = (ReturnCodes)WEXITSTATUS(result);
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
