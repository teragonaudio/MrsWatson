#include <stdlib.h>
#include "base/LinkedList.h"
#include "unit/TestRunner.h"

extern TestSuite addAudioClockTests(void);
extern TestSuite addAudioSettingsTests(void);
extern TestSuite addCharStringTests(void);
extern TestSuite addFileTests(void);
extern TestSuite addFileUtilitiesTests(void);
extern TestSuite addLinkedListTests(void);
extern TestSuite addMidiSequenceTests(void);
extern TestSuite addMidiSourceTests(void);
extern TestSuite addPlatformUtilitiesTests(void);
extern TestSuite addPluginTests(void);
extern TestSuite addPluginChainTests(void);
extern TestSuite addPluginPresetTests(void);
extern TestSuite addPluginVst2xIdTests(void);
extern TestSuite addProgramOptionTests(void);
extern TestSuite addSampleBufferTests(void);
extern TestSuite addSampleSourceTests(void);
extern TestSuite addTaskTimerTests(void);

extern TestSuite addAnalysisClippingTests(void);
extern TestSuite addAnalysisDistortionTests(void);
extern TestSuite addAnalysisSilenceTests(void);

extern void _printTestSummary(int testsRun, int testsPassed, int testsFailed, int testsSkipped);

static void _sumTestSuiteResults(void* item, void* extraData) {
  TestSuite testSuite = (TestSuite)item;
  TestSuite result = (TestSuite)extraData;
  result->numSuccess += testSuite->numSuccess;
  result->numFail += testSuite->numFail;
  result->numSkips += testSuite->numSkips;
}

LinkedList getTestSuites(void);
LinkedList getTestSuites(void) {
  LinkedList internalTestSuites = newLinkedList();
  linkedListAppend(internalTestSuites, addAudioClockTests());
  linkedListAppend(internalTestSuites, addAudioSettingsTests());
  linkedListAppend(internalTestSuites, addCharStringTests());
  linkedListAppend(internalTestSuites, addFileTests());
  linkedListAppend(internalTestSuites, addFileUtilitiesTests());
  linkedListAppend(internalTestSuites, addLinkedListTests());
  linkedListAppend(internalTestSuites, addMidiSequenceTests());
  linkedListAppend(internalTestSuites, addMidiSourceTests());
  linkedListAppend(internalTestSuites, addPlatformUtilitiesTests());
  linkedListAppend(internalTestSuites, addPluginTests());
  linkedListAppend(internalTestSuites, addPluginChainTests());
  linkedListAppend(internalTestSuites, addPluginPresetTests());
  linkedListAppend(internalTestSuites, addPluginVst2xIdTests());
  linkedListAppend(internalTestSuites, addProgramOptionTests());
  linkedListAppend(internalTestSuites, addSampleBufferTests());
  linkedListAppend(internalTestSuites, addSampleSourceTests());
  linkedListAppend(internalTestSuites, addTaskTimerTests());

  linkedListAppend(internalTestSuites, addAnalysisClippingTests());
  linkedListAppend(internalTestSuites, addAnalysisDistortionTests());
  linkedListAppend(internalTestSuites, addAnalysisSilenceTests());

  return internalTestSuites;
}

static void _setTestSuiteOnlyPrintFailing(void* item, void* userData) {
  TestSuite testSuite = (TestSuite)item;
  testSuite->onlyPrintFailing = true;
}

TestSuite runInternalTestSuite(LinkedList testSuites, boolByte onlyPrintFailing);
TestSuite runInternalTestSuite(LinkedList testSuites, boolByte onlyPrintFailing) {
  TestSuite suiteResults;

  if(onlyPrintFailing) {
    linkedListForeach(testSuites, _setTestSuiteOnlyPrintFailing, NULL);
  }
  linkedListForeach(testSuites, runTestSuite, NULL);
  // Create a new test suite to be used as the userData passed to the foreach loop
  suiteResults = newTestSuite("Suite results", NULL, NULL);
  linkedListForeach(testSuites, _sumTestSuiteResults, suiteResults);

  _printTestSummary(suiteResults->numSuccess + suiteResults->numFail + suiteResults->numSkips,
    suiteResults->numSuccess, suiteResults->numFail, suiteResults->numSkips);

  return suiteResults;
}

TestCase findTestCase(TestSuite testSuite, char* testName);
TestCase findTestCase(TestSuite testSuite, char* testName) {
  LinkedList iterator = testSuite->testCases;
  TestCase currentTestCase = NULL;

  while(iterator != NULL) {
    if(iterator->item != NULL) {
      currentTestCase = (TestCase)iterator->item;
      if(!strncasecmp(currentTestCase->name, testName, strlen(currentTestCase->name))) {
        return currentTestCase;
      }
    }
    iterator = iterator->nextItem;
  }

  return NULL;
}

TestSuite findTestSuite(LinkedList testSuites, const CharString testSuiteName);
TestSuite findTestSuite(LinkedList testSuites, const CharString testSuiteName) {
  LinkedList iterator = testSuites;
  TestSuite testSuite = NULL;

  if(testSuiteName == NULL || charStringIsEmpty(testSuiteName)) {
    return NULL;
  }

  while(iterator != NULL) {
    if(iterator->item != NULL) {
      testSuite = (TestSuite)iterator->item;
      if(charStringIsEqualToCString(testSuiteName, testSuite->name, true)) {
        return testSuite;
      }
      else {
        testSuite = NULL;
      }
    }
    iterator = iterator->nextItem;
  }

  return testSuite;
}

static void _printTestCases(void* item, void* userData) {
  TestCase testCase = (TestCase)item;
  char* testSuiteName = (char*)userData;
  printf("%s:%s\n", testSuiteName, testCase->name);
}

static void _printTestsInSuite(void* item, void* userData) {
  TestSuite testSuite = (TestSuite)item;
  linkedListForeach(testSuite->testCases, _printTestCases, testSuite->name);
}

void printInternalTests(void);
void printInternalTests(void) {
  LinkedList internalTestSuites = getTestSuites();
  linkedListForeach(internalTestSuites, _printTestsInSuite, NULL);
  freeLinkedListAndItems(internalTestSuites, (LinkedListFreeItemFunc)freeTestSuite);
}
