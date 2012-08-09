#include "TestRunner.h"
#include "AudioClock.h"

static int const TEST_BLOCKSIZE = 256;

static void _audioClockTestSetup(void) {
  initAudioClock();
}

static void _audioClockTestTeardown(void) {
  freeAudioClock();
}

static int _testInitAudioClock(void) {
  _assertIntEquals(getAudioClockCurrentSample(), 0);
  _assertFalse(getAudioClockIsPlaying());
  _assertFalse(getAudioClockTransportChanged());
  return 0;
}

static int _testAdvanceAudioClock(void) {
  advanceAudioClock(TEST_BLOCKSIZE);
  _assertIntEquals(getAudioClockCurrentSample(), TEST_BLOCKSIZE);
  _assert(getAudioClockIsPlaying());
  _assert(getAudioClockTransportChanged());
  return 0;
}

static int _testStopAudioClock(void) {
  advanceAudioClock(TEST_BLOCKSIZE);
  stopAudioClock();
  _assertFalse(getAudioClockIsPlaying());
  _assert(getAudioClockTransportChanged())
  return 0;
}

static int _testRestartAudioClock(void) {
  advanceAudioClock(TEST_BLOCKSIZE);
  stopAudioClock();
  advanceAudioClock(TEST_BLOCKSIZE);
  _assert(getAudioClockIsPlaying());
  _assert(getAudioClockTransportChanged());
  _assertIntEquals(getAudioClockCurrentSample(), TEST_BLOCKSIZE * 2);
  return 0;
}

static int _testAdvanceClockMulitpleTimes(void) {
  int i;
  for(i = 0; i < 100; i++) {
    advanceAudioClock(TEST_BLOCKSIZE);
  }
  _assert(getAudioClockIsPlaying());
  _assertFalse(getAudioClockTransportChanged());
  _assertIntEquals(getAudioClockCurrentSample(), TEST_BLOCKSIZE * 100);
  return 0;
}

void runAudioClockTests(void);
void runAudioClockTests(void) {
  _startTestSection();
  _runTest("Initialization", _testInitAudioClock, _audioClockTestSetup, _audioClockTestTeardown);
  _runTest("Advance clock", _testAdvanceAudioClock, _audioClockTestSetup, _audioClockTestTeardown);
  _runTest("Stop clock", _testStopAudioClock, _audioClockTestSetup, _audioClockTestTeardown);
  _runTest("Restart clock", _testRestartAudioClock, _audioClockTestSetup, _audioClockTestTeardown);
  _runTest("Multiple advance", _testAdvanceClockMulitpleTimes, _audioClockTestSetup, _audioClockTestTeardown);
}
