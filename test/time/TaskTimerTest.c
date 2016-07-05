//
// TaskTimerTest.c - MrsWatson
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

#include "time/TaskTimer.h"

#include "unit/TestRunner.h"

#include <math.h>

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
  _testTaskTimer =
      newTaskTimerWithCString(TEST_COMPONENT_NAME, TEST_SUBCOMPONENT_NAME);
}

static void _testTaskTimerTeardown(void) { freeTaskTimer(_testTaskTimer); }

static int _testNewTaskTimer(void) {
  CharString c = newCharStringWithCString(TEST_COMPONENT_NAME);
  TaskTimer t = newTaskTimer(c, TEST_SUBCOMPONENT_NAME);

  assert(t->enabled);
  assertCharStringEquals(TEST_COMPONENT_NAME, t->component);
  assertCharStringEquals(TEST_SUBCOMPONENT_NAME, t->subcomponent);
  assertDoubleEquals(0.0, t->totalTaskTime, TEST_DEFAULT_TOLERANCE);

  freeCharString(c);
  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithEmptyComponent(void) {
  CharString c = newCharStringWithCString(EMPTY_STRING);
  TaskTimer t = newTaskTimer(c, TEST_SUBCOMPONENT_NAME);

  assertNotNull(t);
  assertCharStringEquals(EMPTY_STRING, t->component);
  assertCharStringEquals(TEST_SUBCOMPONENT_NAME, t->subcomponent);

  freeCharString(c);
  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithEmptySubcomponent(void) {
  CharString c = newCharStringWithCString(TEST_COMPONENT_NAME);
  TaskTimer t = newTaskTimer(c, EMPTY_STRING);

  assertNotNull(t);
  assertCharStringEquals(TEST_COMPONENT_NAME, t->component);
  assertCharStringEquals(EMPTY_STRING, t->subcomponent);

  freeCharString(c);
  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithNullComponent(void) {
  TaskTimer t = newTaskTimer(NULL, NULL);

  assertNotNull(t);
  assertCharStringEquals(EMPTY_STRING, t->component);
  assertCharStringEquals(EMPTY_STRING, t->subcomponent);

  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithNullComponentCString(void) {
  TaskTimer t = newTaskTimerWithCString(NULL, NULL);

  assertNotNull(t);
  assertCharStringEquals(EMPTY_STRING, t->component);
  assertCharStringEquals(EMPTY_STRING, t->subcomponent);

  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithNullSubcomponent(void) {
  CharString c = newCharStringWithCString(TEST_COMPONENT_NAME);
  TaskTimer t = newTaskTimer(c, NULL);

  assertNotNull(t);
  assertCharStringEquals(TEST_COMPONENT_NAME, t->component);
  assertCharStringEquals(EMPTY_STRING, t->subcomponent);

  freeCharString(c);
  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithCStrings(void) {
  TaskTimer t =
      newTaskTimerWithCString(TEST_COMPONENT_NAME, TEST_SUBCOMPONENT_NAME);

  assert(t->enabled);
  assertCharStringEquals(TEST_COMPONENT_NAME, t->component);
  assertCharStringEquals(TEST_SUBCOMPONENT_NAME, t->subcomponent);
  assertDoubleEquals(0.0, t->totalTaskTime, TEST_DEFAULT_TOLERANCE);

  freeTaskTimer(t);
  return 0;
}

static void _testSleep(void) { taskTimerSleep(SLEEP_DURATION_MS); }

static int _testTaskTimerDuration(void) {
  double elapsedTime = 0.0;
  taskTimerStart(_testTaskTimer);
  _testSleep();
  elapsedTime = taskTimerStop(_testTaskTimer);
  assertTimeEquals(SLEEP_DURATION_MS, _testTaskTimer->totalTaskTime,
                   MAX_TIMER_TOLERANCE_MS);
  assertTimeEquals(SLEEP_DURATION_MS, elapsedTime, MAX_TIMER_TOLERANCE_MS);
  return 0;
}

static int _testTaskTimerDurationMultipleTimes(void) {
  double elapsedTime = 0.0;
  int i;

  for (i = 0; i < 5; i++) {
    taskTimerStart(_testTaskTimer);
    _testSleep();
    elapsedTime = taskTimerStop(_testTaskTimer);
    assertTimeEquals(SLEEP_DURATION_MS, elapsedTime, MAX_TIMER_TOLERANCE_MS);
    elapsedTime = 0.0;
  }

  assertTimeEquals(5.0 * SLEEP_DURATION_MS, _testTaskTimer->totalTaskTime,
                   MAX_TIMER_TOLERANCE_MS * 5.0);

  return 0;
}

static int _testTaskTimerCallStartTwice(void) {
  taskTimerStart(_testTaskTimer);
  taskTimerStart(_testTaskTimer);
  _testSleep();
  taskTimerStop(_testTaskTimer);
  assertTimeEquals(SLEEP_DURATION_MS, _testTaskTimer->totalTaskTime,
                   MAX_TIMER_TOLERANCE_MS);
  return 0;
}

static int _testTaskTimerCallStopTwice(void) {
  taskTimerStart(_testTaskTimer);
  _testSleep();
  taskTimerStop(_testTaskTimer);
  taskTimerStop(_testTaskTimer);
  assertTimeEquals(SLEEP_DURATION_MS, _testTaskTimer->totalTaskTime,
                   MAX_TIMER_TOLERANCE_MS);
  return 0;
}

static int _testCallStopBeforeStart(void) {
  taskTimerStop(_testTaskTimer);
  taskTimerStart(_testTaskTimer);
  _testSleep();
  taskTimerStop(_testTaskTimer);
  assertTimeEquals(SLEEP_DURATION_MS, _testTaskTimer->totalTaskTime,
                   MAX_TIMER_TOLERANCE_MS);
  return 0;
}

static int _testHumanReadableTimeMs(void) {
  CharString s;
  _testTaskTimer->totalTaskTime = 230;
  s = taskTimerHumanReadbleString(_testTaskTimer);
  assertCharStringEquals("230ms", s);
  freeCharString(s);
  return 0;
}

static int _testHumanReadableTimeSec(void) {
  CharString s;
  // 23 seconds
  _testTaskTimer->totalTaskTime = 23000;
  s = taskTimerHumanReadbleString(_testTaskTimer);
  assertCharStringEquals("23sec", s);
  freeCharString(s);
  return 0;
}

static int _testHumanReadableTimeMinSec(void) {
  CharString s;
  // 10 minutes, 23 seconds
  _testTaskTimer->totalTaskTime = 600000 + 23000;
  s = taskTimerHumanReadbleString(_testTaskTimer);
  assertCharStringEquals("10:23sec", s);
  freeCharString(s);
  return 0;
}

static int _testHumanReadableTimeHoursMinSec(void) {
  CharString s;
  // 2 hours, 10 minutes, 23 seconds
  _testTaskTimer->totalTaskTime = (1000 * 60 * 60 * 2) + 600000 + 23000;
  s = taskTimerHumanReadbleString(_testTaskTimer);
  assertCharStringEquals("2:10:23sec", s);
  freeCharString(s);
  return 0;
}

static int _testHumanReadableTimeNotStarted(void) {
  CharString s = taskTimerHumanReadbleString(_testTaskTimer);
  assertCharStringEquals("0ms", s);
  freeCharString(s);
  return 0;
}

static int _testSleepMilliseconds(void) {
  double elapsedTime;
  TaskTimer t = newTaskTimerWithCString("test", "test");
  taskTimerStart(t);
  taskTimerSleep(12);
  elapsedTime = taskTimerStop(t);
  assertTimeEquals(12, elapsedTime, 0.1);
  freeTaskTimer(t);
  return 0;
}

TestSuite addTaskTimerTests(void);
TestSuite addTaskTimerTests(void) {
  TestSuite testSuite =
      newTestSuite("TaskTimer", _testTaskTimerSetup, _testTaskTimerTeardown);
  addTest(testSuite, "NewObject", _testNewTaskTimer);
  addTest(testSuite, "NewObjectWithEmptyComponent",
          _testNewObjectWithEmptyComponent);
  addTest(testSuite, "NewObjectWithEmptySubcomponent",
          _testNewObjectWithEmptySubcomponent);
  addTest(testSuite, "NewObjectWithNullComponent",
          _testNewObjectWithNullComponent);
  addTest(testSuite, "NewObjectWithNullComponentCString",
          _testNewObjectWithNullComponentCString);
  addTest(testSuite, "NewObjectWithNullSubcomponent",
          _testNewObjectWithNullSubcomponent);
  addTest(testSuite, "NewObjectWithCStrings", _testNewObjectWithCStrings);

  addTest(testSuite, "TaskDuration", _testTaskTimerDuration);
  addTest(testSuite, "TaskDurationMultipleTimes",
          _testTaskTimerDurationMultipleTimes);

  addTest(testSuite, "CallStartTwice", _testTaskTimerCallStartTwice);
  addTest(testSuite, "CallStopTwice", _testTaskTimerCallStopTwice);
  addTest(testSuite, "CallStartTwice", _testTaskTimerCallStartTwice);
  addTest(testSuite, "CallStopBeforeStart", _testCallStopBeforeStart);

  addTest(testSuite, "HumanReadableTimeMs", _testHumanReadableTimeMs);
  addTest(testSuite, "HumanReadableTimeSec", _testHumanReadableTimeSec);
  addTest(testSuite, "HumanReadableTimeMinSec", _testHumanReadableTimeMinSec);
  addTest(testSuite, "HumanReadableTimeHoursMinSec",
          _testHumanReadableTimeHoursMinSec);
  addTest(testSuite, "HumanReadableTimeNotStarted",
          _testHumanReadableTimeNotStarted);

  addTest(testSuite, "SleepMilliseconds", _testSleepMilliseconds);
  return testSuite;
}
