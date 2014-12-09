#include <stdlib.h>
#if WINDOWS
#include <io.h>
#endif

#include "unit/TestRunner.h"

void addTestToTestSuite(TestSuite testSuite, TestCase testCase)
{
    linkedListAppend(testSuite->testCases, testCase);
}

const LogColor getLogColor(TestLogEventType eventType)
{
    switch (eventType) {
    case kTestLogEventSection:
        return isatty(1) ? COLOR_FG_CYAN : COLOR_NONE;

    case kTestLogEventPass:
        return isatty(1) ? COLOR_FG_GREEN : COLOR_NONE;

    case kTestLogEventFail:
        return isatty(1) ? COLOR_BG_MAROON : COLOR_NONE;

    case kTestLogEventSkip:
        return isatty(1) ? COLOR_FG_YELLOW : COLOR_NONE;

    case kTestLogEventReset:
        return isatty(1) ? COLOR_RESET : COLOR_NONE;

    default:
        return COLOR_NONE;
    }
}

void printTestSuccess(void)
{
    printToLog(getLogColor(kTestLogEventPass), NULL, "OK");
    flushLog(NULL);
}

void printTestFail(void)
{
    printToLog(getLogColor(kTestLogEventFail), NULL, "FAIL");
    flushLog(NULL);
}

static void _printTestSkipped(void)
{
    printToLog(getLogColor(kTestLogEventSkip), NULL, "Skipped");
    flushLog(NULL);
}

void printTestName(const char *testName)
{
    fprintf(stderr, "  %s: ", testName);
    // Flush standard output in case the test crashes. That way at least the
    // crashing test name is seen.
    fflush(stderr);
}

void runTestCase(void *item, void *extraData)
{
    TestCase testCase = (TestCase)item;
    TestSuite testSuite = (TestSuite)extraData;
    int result;

    if (!testSuite->onlyPrintFailing) {
        printTestName(testCase->name);
    }

    if (testCase->testCaseFunc != NULL) {
        if (testSuite->setup != NULL) {
            testSuite->setup();
        }

        result = testCase->testCaseFunc();

        if (result == 0) {
            if (!testSuite->onlyPrintFailing) {
                printTestSuccess();
            }

            testSuite->numSuccess++;
        } else {
            printTestFail();
            testSuite->numFail++;
        }

        if (testSuite->teardown != NULL) {
            testSuite->teardown();
        }
    } else {
        if (!testSuite->onlyPrintFailing) {
            _printTestSkipped();
        }

        testSuite->numSkips++;
    }
}

void runTestSuite(void *testSuitePtr, void *extraData)
{
    TestSuite testSuite = (TestSuite)testSuitePtr;

    printToLog(getLogColor(kTestLogEventReset), NULL, "Running tests in ");
    printToLog(getLogColor(kTestLogEventSection), NULL, testSuite->name);
    flushLog(NULL);

    linkedListForeach(testSuite->testCases, runTestCase, testSuite);
}

// In both the TestSuite and TestCase objects we assume that we do not need ownership of
// the strings passed in, since they should be allocated on the heap and live for the
// lifetime of the program.
TestSuite newTestSuite(char *name, TestCaseSetupFunc setup, TestCaseTeardownFunc teardown)
{
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
    return testSuite;
}

TestCase newTestCase(char *name, char *filename, int lineNumber, TestCaseExecFunc testCaseFunc)
{
    TestCase testCase = (TestCase)malloc(sizeof(TestCaseMembers));
    testCase->name = name;
    testCase->filename = filename;
    testCase->lineNumber = lineNumber;
    testCase->testCaseFunc = testCaseFunc;
    return testCase;
}

void freeTestCase(TestCase self)
{
    free(self);
}

void freeTestSuite(TestSuite self)
{
    if (self != NULL) {
        freeLinkedListAndItems(self->testCases, (LinkedListFreeItemFunc)freeTestCase);
        free(self);
    }
}
