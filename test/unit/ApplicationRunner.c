#include "base/PlatformUtilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if UNIX
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#include "ApplicationRunner.h"
#include "unit/TestRunner.h"
#include "base/CharString.h"
#include "analysis/AnalyzeFile.h"
#include "base/FileUtilities.h"

const char* kDefaultTestOutputFileType = "pcm";
static const char* kApplicationRunnerOutputFolder = "out";
static const int kApplicationRunnerWaitTimeoutInMs = 1000;

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

CharString buildTestArgumentString(const char* arguments, ...) {
  CharString formattedArguments;
  va_list argumentList;
  va_start(argumentList, arguments);
  formattedArguments = newCharStringWithCapacity(kCharStringLengthLong);
  vsnprintf(formattedArguments->data, formattedArguments->length, arguments, argumentList);
  va_end(argumentList);
  return formattedArguments;
}

char* getTestResourceFilename(const char* resourcesPath, const char* resourceType, const char* resourceName) {
  CharString filename = newCharString();
  snprintf(filename->data, filename->length, "%s%c%s%c%s",
    resourcesPath, PATH_DELIMITER, resourceType, PATH_DELIMITER, resourceName);
  return filename->data;
}

char* getTestOutputFilename(const char* testName, const char* fileExtension) {
  CharString filename = newCharString();
  char* space;
  char *spacePtr;
  snprintf(filename->data, filename->length, "%s%c%s.%s",
    kApplicationRunnerOutputFolder, PATH_DELIMITER, testName, fileExtension);
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

static CharString _getDefaultArguments(TestEnvironment testEnvironment, const char *testName, const char* outputFilename) {
  CharString outString = newCharString();
  snprintf(outString->data, outString->length,
    "--log-file \"%s\" --verbose --output \"%s\" --plugin-root \"%s\"",
    getTestOutputFilename(testName, "txt"), outputFilename,
    _getTestPluginResourcesPath(testEnvironment->resourcesPath));
  return outString;
}

static void _removeOutputFile(char* argument) {
  if(fileExists(argument)) {
    unlink(argument);
  }
}

static void _removeOutputFiles(const char* testName) {
  // Remove all possible output files generated during testing
  _removeOutputFile(getTestOutputFilename(testName, "aif"));
  _removeOutputFile(getTestOutputFilename(testName, "flac"));
  _removeOutputFile(getTestOutputFilename(testName, "pcm"));
  _removeOutputFile(getTestOutputFilename(testName, "wav"));
  _removeOutputFile(getTestOutputFilename(testName, "txt"));
}

static const char* _getResultCodeString(const int resultCode) {
  switch(resultCode) {
    case RETURN_CODE_SUCCESS: return "Success";
    case RETURN_CODE_NOT_RUN: return "Not run";
    case RETURN_CODE_INVALID_ARGUMENT: return "Invalid argument";
    case RETURN_CODE_MISSING_REQUIRED_OPTION: return "Missing required option";
    case RETURN_CODE_IO_ERROR: return "I/O error";
    case RETURN_CODE_PLUGIN_ERROR: return "Plugin error";
    case RETURN_CODE_INVALID_PLUGIN_CHAIN: return "Invalid plugin chain";
    case RETURN_CODE_UNSUPPORTED_FEATURE: return "Unsupported feature";
    case RETURN_CODE_INTERNAL_ERROR: return "Internal error";
    case RETURN_CODE_SIGNAL: return "Caught signal";
    default: return "Unknown";
  }
}

void runApplicationTest(const TestEnvironment testEnvironment,
  const char *testName, CharString testArguments,
  ReturnCodes expectedResultCode, const char* outputFileType)
{
  int result = -1;
  ReturnCodes resultCode = (ReturnCodes)result;
  CharString arguments = newCharStringWithCapacity(kCharStringLengthLong);
  CharString defaultArguments = newCharStringWithCapacity(kCharStringLengthLong);
  CharString failedAnalysisFunctionName = newCharString();
  unsigned long failedAnalysisSample;
  char* outputFilename = getTestOutputFilename(testName,
    outputFileType == NULL ? kDefaultTestOutputFileType : outputFileType);

#if WINDOWS
  STARTUPINFOA startupInfo;
  PROCESS_INFORMATION  processInfo;
#endif

  // Remove files from a previous test run
  _removeOutputFiles(testName);
  makeDirectory(newCharStringWithCString(kApplicationRunnerOutputFolder));

  // Create the command line argument
  charStringAppendCString(arguments, "\"");
  charStringAppendCString(arguments, testEnvironment->applicationPath);
  charStringAppendCString(arguments, "\"");
  charStringAppendCString(arguments, " ");
  defaultArguments = _getDefaultArguments(testEnvironment, testName, outputFilename);
  charStringAppend(arguments, defaultArguments);
  charStringAppendCString(arguments, " ");
  charStringAppend(arguments, testArguments);

  if(!testEnvironment->results->onlyPrintFailing) {
    printTestName(testName);
  }

#if WINDOWS
  memset(&startupInfo, 0, sizeof(startupInfo));
  memset(&processInfo, 0, sizeof(processInfo));
  startupInfo.cb = sizeof(startupInfo);
  result = CreateProcessA((LPCSTR)(testEnvironment->applicationPath), (LPSTR)(arguments->data),
    0, 0, false, CREATE_DEFAULT_ERROR_MODE, 0, 0, &startupInfo, &processInfo);
  if(result) {
    // TODO: Check return codes for these calls
    WaitForSingleObject(processInfo.hProcess, kApplicationRunnerWaitTimeoutInMs);
    GetExitCodeProcess(processInfo.hProcess, (LPDWORD)&resultCode);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
  }
  else {
    logCritical("Could not launch process, got error %s", stringForLastError(GetLastError()));
    return;
  }
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
    if(outputFileType != NULL) {
      if(analyzeFile(outputFilename, failedAnalysisFunctionName, &failedAnalysisSample)) {
        testEnvironment->results->numSuccess++;
        if(!testEnvironment->results->keepFiles) {
          _removeOutputFiles(testName);
        }
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
      if(!testEnvironment->results->keepFiles) {
        _removeOutputFiles(testName);
      }
      if(!testEnvironment->results->onlyPrintFailing) {
        printTestSuccess();
      }
    }
  }
  else {
    if(testEnvironment->results->onlyPrintFailing) {
      printTestName(testName);
    }
    fprintf(stderr, "Expected result code %d (%s), got %d (%s). ",
      expectedResultCode, _getResultCodeString(expectedResultCode),
      resultCode, _getResultCodeString(resultCode));
    printTestFail();
    testEnvironment->results->numFail++;
  }

  freeCharString(arguments);
  freeCharString(defaultArguments);
  freeCharString(testArguments);
  freeCharString(failedAnalysisFunctionName);
}
