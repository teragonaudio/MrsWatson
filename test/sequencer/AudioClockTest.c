#include "unit/TestRunner.h"
#include "sequencer/AudioClock.h"

static const unsigned long kAudioClockTestBlocksize = 256;

static void _audioClockTestSetup(void) {
  initAudioClock();
}

static void _audioClockTestTeardown(void) {
  freeAudioClock(getAudioClock());
}

static int _testInitAudioClock(void) {
  AudioClock audioClock = getAudioClock();
  assertUnsignedLongEquals(audioClock->currentFrame, 0l);
  assertFalse(audioClock->isPlaying);
  assertFalse(audioClock->transportChanged);
  return 0;
}

static int _testAdvanceAudioClock(void) {
  AudioClock audioClock = getAudioClock();
  advanceAudioClock(audioClock, kAudioClockTestBlocksize);
  assertUnsignedLongEquals(audioClock->currentFrame, kAudioClockTestBlocksize);
  assert(audioClock->isPlaying);
  assert(audioClock->transportChanged);
  return 0;
}

static int _testStopAudioClock(void) {
  AudioClock audioClock = getAudioClock();
  advanceAudioClock(audioClock, kAudioClockTestBlocksize);
  audioClockStop(audioClock);
  assertFalse(audioClock->isPlaying);
  assert(audioClock->transportChanged)
  return 0;
}

static int _testRestartAudioClock(void) {
  AudioClock audioClock = getAudioClock();
  advanceAudioClock(audioClock, kAudioClockTestBlocksize);
  audioClockStop(audioClock);
  advanceAudioClock(audioClock, kAudioClockTestBlocksize);
  assert(audioClock->isPlaying);
  assert(audioClock->transportChanged);
  assertUnsignedLongEquals(audioClock->currentFrame, kAudioClockTestBlocksize * 2);
  return 0;
}

static int _testAdvanceClockMulitpleTimes(void) {
  AudioClock audioClock = getAudioClock();
  int i;
  for(i = 0; i < 100; i++) {
    advanceAudioClock(audioClock, kAudioClockTestBlocksize);
  }
  assert(audioClock->isPlaying);
  assertFalse(audioClock->transportChanged);
  assertUnsignedLongEquals(audioClock->currentFrame, kAudioClockTestBlocksize * 100);
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
