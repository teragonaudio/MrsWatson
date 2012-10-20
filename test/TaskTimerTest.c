#include "TestRunner.h"
#include "TaskTimer.h"

static void _taskTimerSetup(void) {
  
}

static void _taskTimerTeardown(void) {
  
}

static int _testNewTaskTimer(void) {
  return 0;
}

static int _testTaskTimerDuration(void) {
  return 0;
}

static int _testTaskTimerCallStartTwice(void) {
  return 0;
}

static int _testTaskTimerCallStopTwice(void) {
  return 0;
}

static int _testCallStopBeforeStart(void) {
  return 0;
}

TestSuite addTaskTimerTests(void);
TestSuite addTaskTimerTests(void) {
  TestSuite testSuite = newTestSuite("TaskTimer", _taskTimerSetup, _taskTimerTeardown);
  addTest(testSuite, "NewObject", NULL); // _testNewTaskTimer);
  addTest(testSuite, "TaskDuration", NULL); // _testTaskTimerDuration);
  addTest(testSuite, "CallStartTwice", NULL); // _testTaskTimerCallStartTwice);
  addTest(testSuite, "CallStopTwice", NULL); // _testTaskTimerCallStopTwice);
  addTest(testSuite, "CallStartTwice", NULL); // _testTaskTimerCallStartTwice);
  addTest(testSuite, "CallStopBeforeStart", NULL); // _testCallStopBeforeStart);
  return testSuite;
}
