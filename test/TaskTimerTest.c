#include <stdlib.h>
#include <math.h>
#include "TestRunner.h"
#include "TaskTimer.h"

#define SLEEP_DURATION_MS 10.0f
// Timer testing is a bit unreliable, so we just check to see that each sleep
// call (see below) is recorded off no more than this amount of milliseconds.
#define MAX_TIMER_TOLERANCE_MS 1.5f

static int _testNewTaskTimer(void) {
  TaskTimer t = newTaskTimer(1);
  assertIntEquals(t->numTasks, 1);
  assertIntEquals(t->currentTask, -1);
  return 0;
}

static int _testTaskTimerDuration(void) {
  TaskTimer t = newTaskTimer(1);
  assertIntEquals(t->currentTask, -1);
  startTimingTask(t, 0);
#if UNIX
  usleep(SLEEP_DURATION_MS * 1000);
#elif WINDOWS
  // TODO: Sleep for 10ms
#endif
  stopTiming(t);
  assertIntEquals(t->currentTask, -1);
  assert(fabsl(10.0 - t->totalTaskTimes[0]) <= MAX_TIMER_TOLERANCE_MS);
  return 0;
}

static int _testTaskTimerDurationMultipleTimes(void) {
  TaskTimer t = newTaskTimer(1);
  int i;
  for(i = 0; i < 5; i++) {
    assertIntEquals(t->currentTask, -1);
    startTimingTask(t, 0);
#if UNIX
    usleep(SLEEP_DURATION_MS * 1000);
#elif WINDOWS
    // TODO: Sleep for 10ms
#endif
    stopTiming(t);    
    assertIntEquals(t->currentTask, -1);
  }
  assert(fabsl(5.0 * 10.0 - t->totalTaskTimes[0]) <= MAX_TIMER_TOLERANCE_MS * 5.0);
  return 0;
}

static int _testTaskTimerCallStartTwice(void) {
  TaskTimer t = newTaskTimer(1);
  startTimingTask(t, 0);
  startTimingTask(t, 0);
#if UNIX
  usleep(SLEEP_DURATION_MS * 1000);
#elif WINDOWS
  // TODO: Sleep for 10ms
#endif
  stopTiming(t);
  assertIntEquals(t->currentTask, -1);
  assert(fabsl(10.0 - t->totalTaskTimes[0]) <= MAX_TIMER_TOLERANCE_MS);
  return 0;
}

static int _testTaskTimerCallStopTwice(void) {
  TaskTimer t = newTaskTimer(1);
  startTimingTask(t, 0);
#if UNIX
  usleep(SLEEP_DURATION_MS * 1000);
#elif WINDOWS
  // TODO: Sleep for 10ms
#endif
  stopTiming(t);
  stopTiming(t);
  assertIntEquals(t->currentTask, -1);
  // Recorded time should be at most 1ms off
  assert(fabsl(10.0 - t->totalTaskTimes[0]) <= MAX_TIMER_TOLERANCE_MS);
  return 0;
}

static int _testCallStopBeforeStart(void) {
  TaskTimer t = newTaskTimer(1);
  stopTiming(t);
  assertIntEquals(t->currentTask, -1);
  startTimingTask(t, 0);
  assertIntEquals(t->currentTask, 0);
#if UNIX
  usleep(SLEEP_DURATION_MS * 1000);
#elif WINDOWS
  // TODO: Sleep for 10ms
#endif
  stopTiming(t);
  assertIntEquals(t->currentTask, -1);
  // Recorded time should be at most 1ms off
  assert(fabsl(10.0 - t->totalTaskTimes[0]) <= MAX_TIMER_TOLERANCE_MS);
  return 0;
}

TestSuite addTaskTimerTests(void);
TestSuite addTaskTimerTests(void) {
  TestSuite testSuite = newTestSuite("TaskTimer", NULL, NULL);
  addTest(testSuite, "NewObject", _testNewTaskTimer);
  addTest(testSuite, "TaskDuration", _testTaskTimerDuration);
  addTest(testSuite, "TaskDurationMultipleTimes", _testTaskTimerDurationMultipleTimes);
  addTest(testSuite, "CallStartTwice", _testTaskTimerCallStartTwice);
  addTest(testSuite, "CallStopTwice", _testTaskTimerCallStopTwice);
  addTest(testSuite, "CallStartTwice", _testTaskTimerCallStartTwice);
  addTest(testSuite, "CallStopBeforeStart", _testCallStopBeforeStart);
  return testSuite;
}
