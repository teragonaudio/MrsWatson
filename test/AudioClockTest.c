#include "AudioClockTest.h"
#include "TestRunner.h"
#include "AudioClock.h"

static void _audioClockTestSetup(void) {
  initAudioClock();
}

static void _audioClockTestTeardown(void) {
  freeAudioClock();
}

static int _testInitAudioClock(void) {
  _assert(getAudioClockCurrentSample() == 0);
  _assert(getAudioClockIsPlaying() == false);
  _assert(getAudioClockTransportChanged() == false);
  return 0;
}

static int _testAdvanceAudioClock(void) {
  advanceAudioClock(100);
  _assert(getAudioClockCurrentSample() == 100);
  _assert(getAudioClockIsPlaying() == true);
  _assert(getAudioClockTransportChanged() == true);
  return 0;
}

int runAudioClockTests(void) {
  int numFailedTests = 0;
  _startTestSection();
  _runTest("Initialization", _testInitAudioClock, _audioClockTestSetup, _audioClockTestTeardown);
  _runTest("Advance clock", _testAdvanceAudioClock, _audioClockTestSetup, _audioClockTestTeardown);
  return numFailedTests;
}
