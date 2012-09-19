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

void runInternalTestSuite(void);
void runInternalTestSuite(void) {
  LinkedList internalTestSuites = newLinkedList();
  appendItemToList(internalTestSuites, addAudioClockTests());
  appendItemToList(internalTestSuites, addAudioSettingsTests());
  appendItemToList(internalTestSuites, addCharStringTests());
  appendItemToList(internalTestSuites, addLinkedListTests());
  appendItemToList(internalTestSuites, addMidiSequenceTests());

  foreachItemInList(internalTestSuites, runTestSuite, NULL);
  _TestSuiteResult suiteResult = malloc(sizeof(_TestSuiteResultMembers));
  suiteResult->numSuccess = 0;
  suiteResult->numFail = 0;
  foreachItemInList(internalTestSuites, _sumTestSuiteResults, suiteResult);

  printf("\nRan %d function tests: %d passed, %d failed\n",
    suiteResult->numSuccess + suiteResult->numFail, suiteResult->numSuccess, suiteResult->numFail);
}