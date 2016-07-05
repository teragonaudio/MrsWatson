//
// AudioSettingsTest.c - MrsWatson
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

#include "audio/AudioSettings.h"

#include "unit/TestRunner.h"

static void _audioSettingsSetup(void) { initAudioSettings(); }

static void _audioSettingsTeardown(void) { freeAudioSettings(); }

static int _testInitAudioSettings(void) {
  assertDoubleEquals(DEFAULT_SAMPLE_RATE, getSampleRate(),
                     TEST_DEFAULT_TOLERANCE);
  assertIntEquals(DEFAULT_NUM_CHANNELS, getNumChannels());
  assertUnsignedLongEquals(DEFAULT_BLOCKSIZE, getBlocksize());
  assertDoubleEquals(DEFAULT_TEMPO, getTempo(), TEST_DEFAULT_TOLERANCE);
  assertIntEquals(DEFAULT_TIMESIG_BEATS_PER_MEASURE,
                  getTimeSignatureBeatsPerMeasure());
  assertIntEquals(DEFAULT_TIMESIG_NOTE_VALUE, getTimeSignatureNoteValue());
  assertIntEquals(16, getBitDepth());
  return 0;
}

static int _testSetSampleRate(void) {
  setSampleRate(22050.0);
  assertDoubleEquals(22050.0, getSampleRate(), TEST_DEFAULT_TOLERANCE);
  return 0;
}

static int _testSetInvalidSampleRate(void) {
  setSampleRate(22050.0);
  assertDoubleEquals(22050.0, getSampleRate(), TEST_DEFAULT_TOLERANCE);
  assertFalse(setSampleRate(0.0));
  assertDoubleEquals(22050.0, getSampleRate(), TEST_DEFAULT_TOLERANCE);
  return 0;
}

static int _testSetNumChannels(void) {
  setNumChannels(4);
  assertIntEquals(4, getNumChannels());
  return 0;
}

static int _testSetInvalidNumChannels(void) {
  setNumChannels(2);
  assertIntEquals(2, getNumChannels());
  assertFalse(setNumChannels(0));
  assertIntEquals(2, getNumChannels());
  return 0;
}

static int _testSetBlocksize(void) {
  setBlocksize(123);
  assertUnsignedLongEquals(123l, getBlocksize());
  return 0;
}

static int _testSetInvalidBlocksize(void) {
  setBlocksize(123);
  assertUnsignedLongEquals(123l, getBlocksize());
  assertFalse(setBlocksize(0));
  assertUnsignedLongEquals(123l, getBlocksize());
  return 0;
}

static int _testSetTempo(void) {
  setTempo(123.45f);
  assertDoubleEquals(123.45, getTempo(), 0.1);
  return 0;
}

static int _testSetInvalidTempo(void) {
  setTempo(100.0);
  assertDoubleEquals(100.0, getTempo(), TEST_DEFAULT_TOLERANCE);
  assertFalse(setTempo(-666.0));
  assertDoubleEquals(100.0, getTempo(), TEST_DEFAULT_TOLERANCE);
  assertFalse(setTempo(0.0));
  assertDoubleEquals(100.0, getTempo(), TEST_DEFAULT_TOLERANCE);
  return 0;
}

static int _testSetTempoWithMidiBytes(void) {
  byte bytes[3];
  bytes[0] = 0x13;
  bytes[1] = 0xe7;
  bytes[2] = 0x1b;
  setTempoFromMidiBytes(bytes);
  assertDoubleEquals(46.0, getTempo(), TEST_DEFAULT_TOLERANCE);
  return 0;
}

static int _testSetTempoWithMidiBytesNull(void) {
  setTempo(100.0);
  assertDoubleEquals(100.0, getTempo(), TEST_DEFAULT_TOLERANCE);
  setTempoFromMidiBytes(NULL);
  assertDoubleEquals(100.0, getTempo(), TEST_DEFAULT_TOLERANCE);
  return 0;
}

static int _testSetTimeSigBeatsPerMeasure(void) {
  assert(setTimeSignatureBeatsPerMeasure(8));
  assertIntEquals(8, getTimeSignatureBeatsPerMeasure());
  return 0;
}

static int _testSetTimeSigNoteValue(void) {
  assert(setTimeSignatureNoteValue(2));
  assertIntEquals(2, getTimeSignatureNoteValue());
  return 0;
}

static int _testSetTimeSignatureWithMidiBytes(void) {
  byte bytes[2];
  // Corresponds to a time signature of 3/8
  bytes[0] = 3;
  bytes[1] = 3;
  assert(setTimeSignatureFromMidiBytes(bytes));
  assertIntEquals(3, getTimeSignatureBeatsPerMeasure());
  assertIntEquals(8, getTimeSignatureNoteValue());
  return 0;
}

static int _testSetTimeSignatureWithMidiBytesNull(void) {
  assert(setTimeSignatureBeatsPerMeasure(3));
  assert(setTimeSignatureNoteValue(8));
  assertIntEquals(3, getTimeSignatureBeatsPerMeasure());
  assertIntEquals(8, getTimeSignatureNoteValue());
  assertFalse(setTimeSignatureFromMidiBytes(NULL));
  assertIntEquals(3, getTimeSignatureBeatsPerMeasure());
  assertIntEquals(8, getTimeSignatureNoteValue());
  return 0;
}

static int _testSetTimeSignatureFromString(void) {
  CharString s = newCharStringWithCString("2/16");
  assert(setTimeSignatureBeatsPerMeasure(3));
  assert(setTimeSignatureNoteValue(8));
  assertIntEquals(3, getTimeSignatureBeatsPerMeasure());
  assertIntEquals(8, getTimeSignatureNoteValue());
  assert(setTimeSignatureFromString(s));
  assertIntEquals(2, getTimeSignatureBeatsPerMeasure());
  assertIntEquals(16, getTimeSignatureNoteValue());
  freeCharString(s);
  return 0;
}

static int _testSetTimeSignatureFromInvalidString(void) {
  CharString s = newCharStringWithCString("invalid/none");
  assert(setTimeSignatureBeatsPerMeasure(3));
  assert(setTimeSignatureNoteValue(8));
  assertIntEquals(3, getTimeSignatureBeatsPerMeasure());
  assertIntEquals(8, getTimeSignatureNoteValue());
  assertFalse(setTimeSignatureFromString(s));
  assertIntEquals(3, getTimeSignatureBeatsPerMeasure());
  assertIntEquals(8, getTimeSignatureNoteValue());
  freeCharString(s);
  return 0;
}

static int _testSetTimeSignatureFromNullString(void) {
  assert(setTimeSignatureBeatsPerMeasure(3));
  assert(setTimeSignatureNoteValue(8));
  assertIntEquals(3, getTimeSignatureBeatsPerMeasure());
  assertIntEquals(8, getTimeSignatureNoteValue());
  assertFalse(setTimeSignatureFromString(NULL));
  assertIntEquals(3, getTimeSignatureBeatsPerMeasure());
  assertIntEquals(8, getTimeSignatureNoteValue());
  return 0;
}

static int _testSetBitDepth(void) {
  setBitDepth(kBitDepth8Bit);
  assertIntEquals(8, getBitDepth());
  setBitDepth(kBitDepth16Bit);
  assertIntEquals(16, getBitDepth());
  setBitDepth(kBitDepth24Bit);
  assertIntEquals(24, getBitDepth());
  setBitDepth(kBitDepth32Bit);
  assertIntEquals(32, getBitDepth());
  return 0;
}

TestSuite addAudioSettingsTests(void);
TestSuite addAudioSettingsTests(void) {
  TestSuite testSuite = newTestSuite("AudioSettings", _audioSettingsSetup,
                                     _audioSettingsTeardown);

  addTest(testSuite, "Initialization", _testInitAudioSettings);
  addTest(testSuite, "SetSampleRate", _testSetSampleRate);
  addTest(testSuite, "SetInvalidSampleRate", _testSetInvalidSampleRate);
  addTest(testSuite, "SetNumChannels", _testSetNumChannels);
  addTest(testSuite, "SetInvalidNumChannels", _testSetInvalidNumChannels);
  addTest(testSuite, "SetBlocksize", _testSetBlocksize);
  addTest(testSuite, "SetInvalidBlocksize", _testSetInvalidBlocksize);
  addTest(testSuite, "SetTempo", _testSetTempo);
  addTest(testSuite, "SetInvalidTempo", _testSetInvalidTempo);
  addTest(testSuite, "SetTempoWithMidiBytes", _testSetTempoWithMidiBytes);
  addTest(testSuite, "SetTempoWithMidiBytesNull",
          _testSetTempoWithMidiBytesNull);

  addTest(testSuite, "SetTimeSignatureBeatsPerMeasure",
          _testSetTimeSigBeatsPerMeasure);
  addTest(testSuite, "SetTimeSignatureNoteValue", _testSetTimeSigNoteValue);
  addTest(testSuite, "SetTimeSignatureWithMidiBytes",
          _testSetTimeSignatureWithMidiBytes);
  addTest(testSuite, "SetTimeSignatureWithMidiBytesNull",
          _testSetTimeSignatureWithMidiBytesNull);
  addTest(testSuite, "SetTimeSignatureFromString",
          _testSetTimeSignatureFromString);
  addTest(testSuite, "SetTimeSignatureFromInvalidString",
          _testSetTimeSignatureFromInvalidString);
  addTest(testSuite, "SetTimeSignatureFromNullString",
          _testSetTimeSignatureFromNullString);

  addTest(testSuite, "SetBitDepth", _testSetBitDepth);

  return testSuite;
}
