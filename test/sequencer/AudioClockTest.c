#include "unit/TestRunner.h"
#include "sequencer/AudioClock.h"

static const unsigned long kAudioClockTestBlocksize = 256;

static void _audioClockTestSetup(void) {
  initAudioClock();
}

static void _audioClockTestTeardown(void) {
  freeAudioClock();
}

static int _testInitAudioClock(void) {
  assertIntEquals(getAudioClockCurrentFrame(), 0);
  assertFalse(getAudioClockIsPlaying());
  assertFalse(getAudioClockTransportChanged());
  return 0;
}

static int _testAdvanceAudioClock(void) {
  advanceAudioClock(kAudioClockTestBlocksize);
  assertIntEquals(getAudioClockCurrentFrame(), kAudioClockTestBlocksize);
  assert(getAudioClockIsPlaying());
  assert(getAudioClockTransportChanged());
  return 0;
}

static int _testStopAudioClock(void) {
  advanceAudioClock(kAudioClockTestBlocksize);
  stopAudioClock();
  assertFalse(getAudioClockIsPlaying());
  assert(getAudioClockTransportChanged())
  return 0;
}

static int _testRestartAudioClock(void) {
  advanceAudioClock(kAudioClockTestBlocksize);
  stopAudioClock();
  advanceAudioClock(kAudioClockTestBlocksize);
  assert(getAudioClockIsPlaying());
  assert(getAudioClockTransportChanged());
  assertIntEquals(getAudioClockCurrentFrame(), kAudioClockTestBlocksize * 2);
  return 0;
}

static int _testAdvanceClockMulitpleTimes(void) {
  int i;
  for(i = 0; i < 100; i++) {
    advanceAudioClock(kAudioClockTestBlocksize);
  }
  assert(getAudioClockIsPlaying());
  assertFalse(getAudioClockTransportChanged());
  assertIntEquals(getAudioClockCurrentFrame(), kAudioClockTestBlocksize * 100);
  return 0;
}

TestSuite addAudioClockTests(void);
TestSuite addAudioClockTests(void) {
  TestSuite testSuite = newTestSuite("AudioClock", _audioClockTestSetup, _audioClockTestTeardown);
  addTest(testSuite, "Initialization", _testInitAudioClock);
  addTest(testSuite, "AdvanceClock", _testAdvanceAudioClock);
  addTest(testSuite, "StopClock", _testStopAudioClock);
  addTest(testSuite, "RestartClock", _testRestartAudioClock);
  addTest(testSuite, "MultipleAdvance", _testAdvanceClockMulitpleTimes);
  return testSuite;
}
