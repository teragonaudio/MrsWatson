#include "AudioClockTest.h"
#include "TestRunner.h"
#include "AudioClock.h"

static int testInitAudioClock(void) {
  initAudioClock();
  _assert(getAudioClockCurrentSample() == 0);
  _assert(getAudioClockIsPlaying() == false);
  _assert(getAudioClockTransportChanged() == false);
  return 0;
}

int runAudioClockTests(void) {
  _runTest(testInitAudioClock);
  return 0;
}