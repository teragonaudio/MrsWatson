#if ! WINDOWS
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif
#include "ApplicationRunner.h"
#include "CharString.h"
#include "AnalyzeFile.h"

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

LinkedList getDefaultArguments(const char *testName) {
  LinkedList arguments = newLinkedList();
  appendItemToList(arguments, "--log-file");
  appendItemToList(arguments, _getTestOutputFilename(testName, "txt"));
  appendItemToList(arguments, "--output");
  appendItemToList(arguments, _getTestOutputFilename(testName, "pcm"));
  return arguments;
}

static void _appendItemToLinkedList(void* item, void* userData) {
  LinkedList linkedList = (LinkedList)userData;
  appendItemToList(linkedList, item);
}

static LinkedList _appendLinkedLists(LinkedList list1, LinkedList list2) {
  LinkedList result = newLinkedList();
  foreachItemInList(list1, _appendItemToLinkedList, result);
  foreachItemInList(list2, _appendItemToLinkedList, result);
  return result;
}

static void _copyArgumentToArray(void* item, void* userData) {
  ArgumentsCopyData* copyData = (ArgumentsCopyData*)userData;
  char* argString = (char*)item;
  size_t argStringLength = strlen(argString);
  copyData->outArray[copyData->currentIndex] = (char*)malloc(sizeof(char) * argStringLength);
  strncpy(copyData->outArray[copyData->currentIndex], argString, argStringLength);
  copyData->currentIndex++;
}

static void _removeOutputFile(void* item, void* userData) {
  char* argument = (char*)item;
  if(argument[0] != '-') {
    unlink(argument);
  }
}

void runApplicationTest(char *applicationPath, const char *testName, LinkedList testArguments, ReturnCodes expectedResultCode, boolByte anazyleOutput) {
  char** applicationArguments;
  ArgumentsCopyData argumentsCopyData;
  int resultCode = -1;
  LinkedList defaultArguments = getDefaultArguments(testName);
  LinkedList arguments = _appendLinkedLists(defaultArguments, testArguments);
  CharString failedAnalysisFunctionName = newCharString();
  unsigned long failedAnalysisSample;

  // Remove any output files which may have been left from previous tests
  foreachItemInList(defaultArguments, _removeOutputFile, NULL);

#if WINDOWS
#else
  mkdir("out", 0755);
#endif

#if WINDOWS
  logUnsupportedFeature("Application testing");
#else
  int numArgs = numItemsInList(arguments);
  // Add two extra items to the array, one for the application path and another for a NULL object.
  // These are required for the calls to the execv* functions.
  applicationArguments = (char**)malloc(sizeof(char*) * (numArgs + 2));
  applicationArguments[0] = applicationPath;
  applicationArguments[numArgs + 1] = NULL;
  argumentsCopyData.currentIndex = 1;
  argumentsCopyData.outArray = applicationArguments;
  foreachItemInList(arguments, _copyArgumentToArray, &argumentsCopyData);
  printf("  %s: ", testName);

  pid_t forkedPid = fork();
  if(forkedPid == 0) {
    resultCode = execvp(applicationPath, applicationArguments);
    exit(resultCode);
  }
  else {
    int statusLoc;
    waitpid(forkedPid, &statusLoc, 0);
    if(WIFEXITED(statusLoc)) {
      resultCode = WEXITSTATUS(statusLoc);
    }
  }
#endif

  if(resultCode == expectedResultCode) {
    if(anazyleOutput) {
      if(analyzeFile(_getTestOutputFilename(testName, "pcm"), failedAnalysisFunctionName, &failedAnalysisSample)) {
        testsPassed++;
        foreachItemInList(defaultArguments, _removeOutputFile, NULL);
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
      foreachItemInList(defaultArguments, _removeOutputFile, NULL);
      printTestSuccess();
    }
  }
  else {
    printTestFail();
    printf("    in %s. Expected result code %d, got %d.\n", testName, expectedResultCode, resultCode);
    testsFailed++;
  }

  freeLinkedList(defaultArguments);
  freeLinkedList(testArguments);
  freeLinkedList(arguments);
}
