//
// TestRunner.c - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include "TestRunner.h"

#include "time/AudioClock.h"

#include <stdlib.h>
#if WINDOWS
#include <io.h>
#endif

static boolByte gUseColor = false;

void useColoredOutput(const CharString state) {
  if (charStringIsEqualToCString(state, "force", true)) {
    gUseColor = true;
  } else if (charStringIsEqualToCString(state, "none", true)) {
    gUseColor = false;
  } else if (charStringIsEqualToCString(state, "auto", true)) {
    gUseColor = (boolByte)isatty(1);
  } else {
    // Some other weird string --> no color for you!
    gUseColor = false;
  }
}

void addTestToTestSuite(TestSuite testSuite, TestCase testCase) {
  linkedListAppend(testSuite->testCases, testCase);
}

const LogColor getLogColor(TestLogEventType eventType) {
  switch (eventType) {
  case kTestLogEventSection:
    return gUseColor ? COLOR_FG_CYAN : COLOR_NONE;

  case kTestLogEventPass:
    return gUseColor ? COLOR_FG_GREEN : COLOR_NONE;

  case kTestLogEventFail:
    return gUseColor ? COLOR_BG_MAROON : COLOR_NONE;

  case kTestLogEventSkip:
    return gUseColor ? COLOR_FG_YELLOW : COLOR_NONE;

  case kTestLogEventReset:
    return gUseColor ? COLOR_RESET : COLOR_NONE;

  default:
    return COLOR_NONE;
  }
}

void printTestSuccess(void) {
  printToLog(getLogColor(kTestLogEventPass), NULL, "OK");
  flushLog(NULL);
}

void printTestFail(void) {
  printToLog(getLogColor(kTestLogEventFail), NULL, "FAIL");
  flushLog(NULL);
}

static void _printTestSkipped(void) {
  printToLog(getLogColor(kTestLogEventSkip), NULL, "Skipped");
  flushLog(NULL);
}

void printTestName(const char *testName) {
  fprintf(stderr, "  %s: ", testName);
  // Flush standard output in case the test crashes. That way at least the
  // crashing test name is seen.
  fflush(stderr);
}

void runTestCase(void *item, void *extraData) {
  TestCase testCase = (TestCase)item;
  TestSuite testSuite = (TestSuite)extraData;
  int result = 0;

  if (!testSuite->onlyPrintFailing) {
    printTestName(testCase->name);
  }

  if (testCase->testCaseFunc != NULL ||
      testCase->testCaseWithPathsFunc != NULL) {
    if (testSuite->setup != NULL) {
      testSuite->setup();
    }

    if (testCase->testCaseWithPathsFunc != NULL) {
      result = testCase->testCaseWithPathsFunc(
          testCase->name, testSuite->applicationPath, testSuite->resourcesPath);
    } else {
      result = testCase->testCaseFunc();
    }

    if (result == 0) {
      testSuite->numSuccess++;

      if (!testSuite->onlyPrintFailing) {
        printTestSuccess();
      }
    } else if (result == -1) {
      testSuite->numSkips++;

      if (!testSuite->onlyPrintFailing) {
        _printTestSkipped();
      }
    } else {
      printTestFail();
      testSuite->numFail++;
    }

    if (testSuite->teardown != NULL) {
      testSuite->teardown();
    }
  } else {
    testSuite->numSkips++;

    if (!testSuite->onlyPrintFailing) {
      _printTestSkipped();
    }
  }
}

void runTestSuite(void *testSuitePtr, void *extraData) {
  TestSuite testSuite = (TestSuite)testSuitePtr;

  // TODO: Remove this hack when AudioClock is no longer a singleton
  // The problem here is that audio clock is also used by the event logger to
  // show the
  // current sample frame. Unfortunately, the teardown method in the AudioClock
  // test
  // suite will destroy the global instance for us.
  initAudioClock();

  printToLog(getLogColor(kTestLogEventReset), NULL, "Running tests in ");
  printToLog(getLogColor(kTestLogEventSection), NULL, testSuite->name);
  flushLog(NULL);

  linkedListForeach(testSuite->testCases, runTestCase, testSuite);
}

// In both the TestSuite and TestCase objects we assume that we do not need
// ownership of
// the strings passed in, since they should be allocated on the heap and live
// for the
// lifetime of the program.
TestSuite newTestSuite(char *name, TestCaseSetupFunc setup,
                       TestCaseTeardownFunc teardown) {
  TestSuite testSuite = (TestSuite)malloc(sizeof(TestSuiteMembers));
  testSuite->name = name;
  testSuite->numSuccess = 0;
  testSuite->numFail = 0;
  testSuite->numSkips = 0;
  testSuite->testCases = newLinkedList();
  testSuite->setup = setup;
  testSuite->teardown = teardown;
  testSuite->onlyPrintFailing = false;
  testSuite->keepFiles = false;
  testSuite->applicationPath = NULL;
  testSuite->resourcesPath = NULL;
  return testSuite;
}

TestCase newTestCase(char *name, char *filename, int lineNumber,
                     TestCaseExecFunc testCaseFunc) {
  TestCase testCase = (TestCase)malloc(sizeof(TestCaseMembers));
  testCase->name = name;
  testCase->filename = filename;
  testCase->lineNumber = lineNumber;
  testCase->testCaseFunc = testCaseFunc;
  testCase->testCaseWithPathsFunc = NULL;
  return testCase;
}

TestCase newTestCaseWithPaths(char *name, char *filename, int lineNumber,
                              TestCaseExecWithPathsFunc testCaseWithPathsFunc) {
  TestCase testCase = (TestCase)malloc(sizeof(TestCaseMembers));
  testCase->name = name;
  testCase->filename = filename;
  testCase->lineNumber = lineNumber;
  testCase->testCaseFunc = NULL;
  testCase->testCaseWithPathsFunc = testCaseWithPathsFunc;
  return testCase;
}

void freeTestCase(TestCase self) { free(self); }

void freeTestSuite(TestSuite self) {
  if (self != NULL) {
    freeLinkedListAndItems(self->testCases,
                           (LinkedListFreeItemFunc)freeTestCase);
    freeCharString(self->applicationPath);
    freeCharString(self->resourcesPath);
    free(self);
  }
}
