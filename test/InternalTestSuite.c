#include <stdlib.h>
#include "LinkedList.h"
#include "TestRunner.h"

extern void runAudioClockTests(void);
extern void runAudioSettingsTests(void);
extern void runCharStringTests(void);
extern void runLinkedListTests(void);
extern void runMidiSequenceTests(void);

extern TestSuite addAudioClockTests(void);

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
#if 1
  LinkedList internalTestSuites = newLinkedList();
  appendItemToList(internalTestSuites, addAudioClockTests());
  foreachItemInList(internalTestSuites, runTestSuite, NULL);
  _TestSuiteResult suiteResult = malloc(sizeof(_TestSuiteResultMembers));
  suiteResult->numSuccess = 0;
  suiteResult->numFail = 0;
  foreachItemInList(internalTestSuites, _sumTestSuiteResults, suiteResult);

  printf("\nRan %d function tests: %d passed, %d failed\n",
    suiteResult->numSuccess + suiteResult->numFail, suiteResult->numSuccess, suiteResult->numFail);
  #else
  runAudioClockTests();
  runAudioSettingsTests();
  runCharStringTests();
  runLinkedListTests();
  runMidiSequenceTests();
  #endif
}