#include <stdlib.h>
#include "LinkedList.h"
#include "TestRunner.h"

extern TestSuite addAudioClockTests(void);
extern TestSuite addAudioSettingsTests(void);
extern TestSuite addCharStringTests(void);
extern TestSuite addLinkedListTests(void);
extern TestSuite addMidiSequenceTests(void);

typedef struct {
  int numSuccess;
  int numFail;
} _TestSuiteResultMembers;
typedef _TestSuiteResultMembers* _TestSuiteResult;

static void _sumTestSuiteResults(void* item, void* extraData) {
  TestSuite testSuite = (TestSuite)item;
  _TestSuiteResult result = (_TestSuiteResult)extraData;
  result->numSuccess += testSuite->numSuccess;
  result->numFail += testSuite->numFail;
}

static LinkedList _getTestSuites(void) {
  LinkedList internalTestSuites = newLinkedList();
  appendItemToList(internalTestSuites, addAudioClockTests());
  appendItemToList(internalTestSuites, addAudioSettingsTests());
  appendItemToList(internalTestSuites, addCharStringTests());
  appendItemToList(internalTestSuites, addLinkedListTests());
  appendItemToList(internalTestSuites, addMidiSequenceTests());
  return internalTestSuites;
}

void runInternalTestSuite(void);
void runInternalTestSuite(void) {
  LinkedList internalTestSuites = _getTestSuites();

  foreachItemInList(internalTestSuites, runTestSuite, NULL);
  _TestSuiteResult suiteResult = malloc(sizeof(_TestSuiteResultMembers));
  suiteResult->numSuccess = 0;
  suiteResult->numFail = 0;
  foreachItemInList(internalTestSuites, _sumTestSuiteResults, suiteResult);

  printf("\nRan %d function tests: %d passed, %d failed\n",
    suiteResult->numSuccess + suiteResult->numFail, suiteResult->numSuccess, suiteResult->numFail);
}

TestCase findTestCase(TestSuite testSuite, char* testName);
TestCase findTestCase(TestSuite testSuite, char* testName) {
  LinkedList iterator = testSuite->testCases;
  TestCase currentTestCase = NULL;

  while(iterator != NULL) {
    if(iterator->item != NULL) {
      currentTestCase = (TestCase)iterator->item;
      if(!strncasecmp(testName, currentTestCase->name, true)) {
        return currentTestCase;
      }
    }
    iterator = iterator->nextItem;
  }

  return NULL;
}

TestSuite findTestSuite(char *testSuiteName);
TestSuite findTestSuite(char *testSuiteName) {
  LinkedList internalTestSuites = _getTestSuites();
  LinkedList iterator = internalTestSuites;
  TestSuite testSuite = NULL;

  while(iterator != NULL) {
    if(iterator->item != NULL) {
      testSuite = (TestSuite)iterator->item;
      if(!strncasecmp(testSuite->name, testSuiteName, STRING_LENGTH_DEFAULT)) {
        return testSuite;
      }
    }
    iterator = iterator->nextItem;
  }

  return NULL;
}
