#include "base/PlatformUtilities.h"

#include <stdlib.h>
#include <math.h>
#if UNIX
#include <unistd.h>
#include <time.h>
#endif

#include "unit/TestRunner.h"
#include "time/TaskTimer.h"

#define SLEEP_DURATION_MS 10.0
// Timer testing is a bit unreliable, so we just check to see that each sleep
// call (see below) is recorded off no more than this amount of milliseconds.
#define MAX_TIMER_TOLERANCE_MS 1.5f
#define TEST_COMPONENT_NAME "component"
#define TEST_SUBCOMPONENT_NAME "subcomponent"

// Keep a static task timer which can easily be destroyed in the teardown. The
// reason for this is that when running the test suite in valgrind, the timing
// tests often fail, which will cuase the suite to leak. Having failed timing
// tests is probably ok when running in valgrind, but leaking memory isn't, as
// that's the entire point of running it there. :)
static TaskTimer _testTaskTimer;

static void _testTaskTimerSetup(void) {
  _testTaskTimer = newTaskTimerWithCString(TEST_COMPONENT_NAME, TEST_SUBCOMPONENT_NAME);
}

static void _testTaskTimerTeardown(void) {
  freeTaskTimer(_testTaskTimer);
}

static int _testNewTaskTimer(void) {
  CharString c = newCharStringWithCString(TEST_COMPONENT_NAME);
  TaskTimer t = newTaskTimer(c, TEST_SUBCOMPONENT_NAME);

  assert(t->enabled);
  assertCharStringEquals(t->component, TEST_COMPONENT_NAME);
  assertCharStringEquals(t->subcomponent, TEST_SUBCOMPONENT_NAME);
  assertDoubleEquals(t->totalTaskTime, 0.0, TEST_FLOAT_TOLERANCE);

  freeCharString(c);
  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithEmptyComponent(void) {
  CharString c = newCharStringWithCString(EMPTY_STRING);
  TaskTimer t = newTaskTimer(c, TEST_SUBCOMPONENT_NAME);

  assertNotNull(t);
  assertCharStringEquals(t->component, EMPTY_STRING);
  assertCharStringEquals(t->subcomponent, TEST_SUBCOMPONENT_NAME);

  freeCharString(c);
  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithEmptySubcomponent(void) {
  CharString c = newCharStringWithCString(TEST_COMPONENT_NAME);
  TaskTimer t = newTaskTimer(c, EMPTY_STRING);

  assertNotNull(t);
  assertCharStringEquals(t->component, TEST_COMPONENT_NAME);
  assertCharStringEquals(t->subcomponent, EMPTY_STRING);

  freeCharString(c);
  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithNullComponent(void) {
  TaskTimer t = newTaskTimer(NULL, NULL);

  assertNotNull(t);
  assertCharStringEquals(t->component, EMPTY_STRING);
  assertCharStringEquals(t->subcomponent, EMPTY_STRING);

  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithNullComponentCString(void) {
  TaskTimer t = newTaskTimerWithCString(NULL, NULL);

  assertNotNull(t);
  assertCharStringEquals(t->component, EMPTY_STRING);
  assertCharStringEquals(t->subcomponent, EMPTY_STRING);

  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithNullSubcomponent(void) {
  CharString c = newCharStringWithCString(TEST_COMPONENT_NAME);
  TaskTimer t = newTaskTimer(c, NULL);

  assertNotNull(t);
  assertCharStringEquals(t->component, TEST_COMPONENT_NAME);
  assertCharStringEquals(t->subcomponent, EMPTY_STRING);

  freeCharString(c);
  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithCStrings(void) {
  TaskTimer t = newTaskTimerWithCString(TEST_COMPONENT_NAME, TEST_SUBCOMPONENT_NAME);

  assert(t->enabled);
  assertCharStringEquals(t->component, TEST_COMPONENT_NAME);
  assertCharStringEquals(t->subcomponent, TEST_SUBCOMPONENT_NAME);
  assertDoubleEquals(t->totalTaskTime, 0.0, TEST_FLOAT_TOLERANCE);

  freeTaskTimer(t);
  return 0;
}

static void _testSleep(void) {
#if UNIX
  struct timespec sleepTime;
  sleepTime.tv_sec = 0;
  sleepTime.tv_nsec = 1000000 * SLEEP_DURATION_MS;
  nanosleep(&sleepTime, NULL);
#elif WINDOWS
  Sleep((DWORD)SLEEP_DURATION_MS);
#endif
}

static int _testTaskTimerDuration(void) {
  taskTimerStart(_testTaskTimer);
  _testSleep();
  taskTimerStop(_testTaskTimer);
  assertDoubleEquals(_testTaskTimer->totalTaskTime, SLEEP_DURATION_MS, MAX_TIMER_TOLERANCE_MS);
  return 0;
}

static int _testTaskTimerDurationMultipleTimes(void) {
  int i;

  for(i = 0; i < 5; i++) {
    taskTimerStart(_testTaskTimer);
    _testSleep();
    taskTimerStop(_testTaskTimer);
  }
  assertDoubleEquals(_testTaskTimer->totalTaskTime, 5.0 * SLEEP_DURATION_MS, MAX_TIMER_TOLERANCE_MS * 5.0);

  return 0;
}

static int _testTaskTimerCallStartTwice(void) {
  taskTimerStart(_testTaskTimer);
  taskTimerStart(_testTaskTimer);
  _testSleep();
  taskTimerStop(_testTaskTimer);
  assertDoubleEquals(_testTaskTimer->totalTaskTime, SLEEP_DURATION_MS, MAX_TIMER_TOLERANCE_MS);
  return 0;
}

static int _testTaskTimerCallStopTwice(void) {
  taskTimerStart(_testTaskTimer);
  _testSleep();
  taskTimerStop(_testTaskTimer);
  taskTimerStop(_testTaskTimer);
  assertDoubleEquals(_testTaskTimer->totalTaskTime, SLEEP_DURATION_MS, MAX_TIMER_TOLERANCE_MS);
  return 0;
}

static int _testCallStopBeforeStart(void) {
  taskTimerStop(_testTaskTimer);
  taskTimerStart(_testTaskTimer);
  _testSleep();
  taskTimerStop(_testTaskTimer);
  assertDoubleEquals(_testTaskTimer->totalTaskTime, SLEEP_DURATION_MS, MAX_TIMER_TOLERANCE_MS);
  return 0;
}

static int _testHumanReadableTimeMs(void) {
  CharString s;
  _testTaskTimer->totalTaskTime = 230;
  s = taskTimerHumanReadbleString(_testTaskTimer);
  assertCharStringEquals(s, "230ms");
  freeCharString(s);
  return 0;
}

static int _testHumanReadableTimeSec(void) {
  CharString s;
  // 23 seconds
  _testTaskTimer->totalTaskTime = 23000;
  s = taskTimerHumanReadbleString(_testTaskTimer);
  assertCharStringEquals(s, "23sec");
  freeCharString(s);
  return 0;
}

static int _testHumanReadableTimeMinSec(void) {
  CharString s;
  // 10 minutes, 23 seconds
  _testTaskTimer->totalTaskTime = 600000 + 23000;
  s = taskTimerHumanReadbleString(_testTaskTimer);
  assertCharStringEquals(s, "10:23sec");
  freeCharString(s);
  return 0;
}

static int _testHumanReadableTimeHoursMinSec(void) {
  CharString s;
  // 2 hours, 10 minutes, 23 seconds
  _testTaskTimer->totalTaskTime = (1000 * 60 * 60 * 2) + 600000 + 23000;
  s = taskTimerHumanReadbleString(_testTaskTimer);
  assertCharStringEquals(s, "2:10:23sec");
  freeCharString(s);
  return 0;
}

static int _testHumanReadableTimeNotStarted(void) {
  CharString s = taskTimerHumanReadbleString(_testTaskTimer);
  assertCharStringEquals(s, "0ms");
  freeCharString(s);
  return 0;
}

TestSuite addTaskTimerTests(void);
TestSuite addTaskTimerTests(void) {
  TestSuite testSuite = newTestSuite("TaskTimer", _testTaskTimerSetup, _testTaskTimerTeardown);
  addTest(testSuite, "NewObject", _testNewTaskTimer);
  addTest(testSuite, "NewObjectWithEmptyComponent", _testNewObjectWithEmptyComponent);
  addTest(testSuite, "NewObjectWithEmptySubcomponent", _testNewObjectWithEmptySubcomponent);
  addTest(testSuite, "NewObjectWithNullComponent", _testNewObjectWithNullComponent);
  addTest(testSuite, "NewObjectWithNullComponentCString", _testNewObjectWithNullComponentCString);
  addTest(testSuite, "NewObjectWithNullSubcomponent", _testNewObjectWithNullSubcomponent);
  addTest(testSuite, "NewObjectWithCStrings", _testNewObjectWithCStrings);
  addTest(testSuite, "TaskDuration", _testTaskTimerDuration);
  addTest(testSuite, "TaskDurationMultipleTimes", _testTaskTimerDurationMultipleTimes);
  addTest(testSuite, "CallStartTwice", _testTaskTimerCallStartTwice);
  addTest(testSuite, "CallStopTwice", _testTaskTimerCallStopTwice);
  addTest(testSuite, "CallStartTwice", _testTaskTimerCallStartTwice);
  addTest(testSuite, "CallStopBeforeStart", _testCallStopBeforeStart);
  addTest(testSuite, "HumanReadableTimeMs", _testHumanReadableTimeMs);
  addTest(testSuite, "HumanReadableTimeSec", _testHumanReadableTimeSec);
  addTest(testSuite, "HumanReadableTimeMinSec", _testHumanReadableTimeMinSec);
  addTest(testSuite, "HumanReadableTimeHoursMinSec", _testHumanReadableTimeHoursMinSec);
  addTest(testSuite, "HumanReadableTimeNotStarted", _testHumanReadableTimeNotStarted);
  return testSuite;
}
