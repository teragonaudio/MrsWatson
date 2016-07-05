//
// AudioClockTest.c - MrsWatson
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

#include "time/AudioClock.h"

#include "unit/TestRunner.h"

static const unsigned long kAudioClockTestBlocksize = 256;

static void _audioClockTestSetup(void) { initAudioClock(); }

static void _audioClockTestTeardown(void) { freeAudioClock(getAudioClock()); }

static int _testInitAudioClock(void) {
  AudioClock audioClock = getAudioClock();
  assertUnsignedLongEquals(ZERO_UNSIGNED_LONG, audioClock->currentFrame);
  assertFalse(audioClock->isPlaying);
  assertFalse(audioClock->transportChanged);
  return 0;
}

static int _testAdvanceAudioClock(void) {
  AudioClock audioClock = getAudioClock();
  advanceAudioClock(audioClock, kAudioClockTestBlocksize);
  assertUnsignedLongEquals(kAudioClockTestBlocksize, audioClock->currentFrame);
  assert(audioClock->isPlaying);
  assert(audioClock->transportChanged);
  return 0;
}

static int _testStopAudioClock(void) {
  AudioClock audioClock = getAudioClock();
  advanceAudioClock(audioClock, kAudioClockTestBlocksize);
  audioClockStop(audioClock);
  assertFalse(audioClock->isPlaying);
  assert(audioClock->transportChanged) return 0;
}

static int _testRestartAudioClock(void) {
  AudioClock audioClock = getAudioClock();
  advanceAudioClock(audioClock, kAudioClockTestBlocksize);
  audioClockStop(audioClock);
  advanceAudioClock(audioClock, kAudioClockTestBlocksize);
  assert(audioClock->isPlaying);
  assert(audioClock->transportChanged);
  assertUnsignedLongEquals(kAudioClockTestBlocksize * 2,
                           audioClock->currentFrame);
  return 0;
}

static int _testAdvanceClockMulitpleTimes(void) {
  AudioClock audioClock = getAudioClock();
  int i;

  for (i = 0; i < 100; i++) {
    advanceAudioClock(audioClock, kAudioClockTestBlocksize);
  }

  assert(audioClock->isPlaying);
  assertFalse(audioClock->transportChanged);
  assertUnsignedLongEquals(kAudioClockTestBlocksize * 100,
                           audioClock->currentFrame);
  return 0;
}

TestSuite addAudioClockTests(void);
TestSuite addAudioClockTests(void) {
  TestSuite testSuite =
      newTestSuite("AudioClock", _audioClockTestSetup, _audioClockTestTeardown);
  addTest(testSuite, "Initialization", _testInitAudioClock);
  addTest(testSuite, "AdvanceClock", _testAdvanceAudioClock);
  addTest(testSuite, "StopClock", _testStopAudioClock);
  addTest(testSuite, "RestartClock", _testRestartAudioClock);
  addTest(testSuite, "MultipleAdvance", _testAdvanceClockMulitpleTimes);
  return testSuite;
}
