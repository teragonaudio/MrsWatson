#include "audio/AudioSettings.h"
#include "unit/TestRunner.h"

static void _audioSettingsSetup(void) {
  initAudioSettings();
}

static void _audioSettingsTeardown(void) {
  freeAudioSettings();
}

static int _testInitAudioSettings(void) {
  assertDoubleEquals(getSampleRate(), DEFAULT_SAMPLE_RATE, TEST_FLOAT_TOLERANCE);
  assertIntEquals(getNumChannels(), DEFAULT_NUM_CHANNELS);
  assertUnsignedLongEquals(getBlocksize(), DEFAULT_BLOCKSIZE);
  assertDoubleEquals(getTempo(), DEFAULT_TEMPO, TEST_FLOAT_TOLERANCE);
  assertIntEquals(getTimeSignatureBeatsPerMeasure(), DEFAULT_TIMESIG_BEATS_PER_MEASURE);
  assertIntEquals(getTimeSignatureNoteValue(), DEFAULT_TIMESIG_NOTE_VALUE);
  return 0;
}

static int _testSetSampleRate(void) {
  setSampleRate(22050.0);
  assertDoubleEquals(getSampleRate(), 22050.0, TEST_FLOAT_TOLERANCE);
  return 0;
}

static int _testSetInvalidSampleRate(void) {
  setSampleRate(22050.0);
  assertDoubleEquals(getSampleRate(), 22050.0, TEST_FLOAT_TOLERANCE);
  setSampleRate(0.0);
  assertDoubleEquals(getSampleRate(), 22050.0, TEST_FLOAT_TOLERANCE);
  return 0;
}

static int _testSetNumChannels(void) {
  setNumChannels(4);
  assertIntEquals(getNumChannels(), 4);
  return 0;
}

static int _testSetInvalidNumChannels(void) {
  setNumChannels(2);
  assertIntEquals(getNumChannels(), 2);
  setNumChannels(0);
  assertIntEquals(getNumChannels(), 2);
  return 0;
}

static int _testSetBlocksize(void) {
  setBlocksize(123);
  assertUnsignedLongEquals(getBlocksize(), 123l);
  return 0;
}

static int _testSetInvalidBlocksize(void) {
  setBlocksize(123);
  assertUnsignedLongEquals(getBlocksize(), 123l);
  setBlocksize(0);
  assertUnsignedLongEquals(getBlocksize(), 123l);
  return 0;
}

static int _testSetTempo(void) {
  setTempo(123.45);
  assertDoubleEquals(getTempo(), 123.45, TEST_FLOAT_TOLERANCE);
  return 0;
}

static int _testSetInvalidTempo(void) {
  setTempo(100.0);
  assertDoubleEquals(getTempo(), 100.0, TEST_FLOAT_TOLERANCE);
  setTempo(-666.0);
  assertDoubleEquals(getTempo(), 100.0, TEST_FLOAT_TOLERANCE);
  setTempo(0.0);
  assertDoubleEquals(getTempo(), 100.0, TEST_FLOAT_TOLERANCE);
  return 0;
}

static int _testSetTempoWithMidiBytes(void) {
  byte bytes[3];
  bytes[0] = 0x13;
  bytes[1] = 0xe7;
  bytes[2] = 0x1b;
  setTempoFromMidiBytes(bytes);
  assertDoubleEquals(getTempo(), 46.0, TEST_FLOAT_TOLERANCE);
  return 0;
}

static int _testSetTempoWithMidiBytesNull(void) {
  setTempo(100.0);
  assertDoubleEquals(getTempo(), 100.0, TEST_FLOAT_TOLERANCE);
  setTempoFromMidiBytes(NULL);
  assertDoubleEquals(getTempo(), 100.0, TEST_FLOAT_TOLERANCE);
  return 0;
}

static int _testSetTimeSigBeatsPerMeasure(void) {
  assert(setTimeSignatureBeatsPerMeasure(8));
  assertIntEquals(getTimeSignatureBeatsPerMeasure(), 8);
  return 0;
}

static int _testSetTimeSigNoteValue(void) {
  assert(setTimeSignatureNoteValue(2));
  assertIntEquals(getTimeSignatureNoteValue(), 2);
  return 0;
}

static int _testSetTimeSignatureWithMidiBytes(void) {
  byte bytes[2];
  // Corresponds to a time signature of 3/8
  bytes[0] = 3;
  bytes[1] = 3;
  assert(setTimeSignatureFromMidiBytes(bytes));
  assertIntEquals(getTimeSignatureBeatsPerMeasure(), 3);
  assertIntEquals(getTimeSignatureNoteValue(), 8);
  return 0;
}

static int _testSetTimeSignatureWithMidiBytesNull(void) {
  assert(setTimeSignatureBeatsPerMeasure(3));
  assert(setTimeSignatureNoteValue(8));
  assertIntEquals(getTimeSignatureBeatsPerMeasure(), 3);
  assertIntEquals(getTimeSignatureNoteValue(), 8);
  assertFalse(setTimeSignatureFromMidiBytes(NULL));
  assertIntEquals(getTimeSignatureBeatsPerMeasure(), 3);
  assertIntEquals(getTimeSignatureNoteValue(), 8);
  return 0;
}

static int _testSetTimeSignatureFromString(void) {
  CharString s = newCharStringWithCString("2/16");
  assert(setTimeSignatureBeatsPerMeasure(3));
  assert(setTimeSignatureNoteValue(8));
  assertIntEquals(getTimeSignatureBeatsPerMeasure(), 3);
  assertIntEquals(getTimeSignatureNoteValue(), 8);
  assert(setTimeSignatureFromString(s));
  assertIntEquals(getTimeSignatureBeatsPerMeasure(), 2);
  assertIntEquals(getTimeSignatureNoteValue(), 16);
  freeCharString(s);
  return 0;
}

static int _testSetTimeSignatureFromInvalidString(void) {
  CharString s = newCharStringWithCString("invalid/none");
  assert(setTimeSignatureBeatsPerMeasure(3));
  assert(setTimeSignatureNoteValue(8));
  assertIntEquals(getTimeSignatureBeatsPerMeasure(), 3);
  assertIntEquals(getTimeSignatureNoteValue(), 8);
  assertFalse(setTimeSignatureFromString(s));
  assertIntEquals(getTimeSignatureBeatsPerMeasure(), 3);
  assertIntEquals(getTimeSignatureNoteValue(), 8);
  freeCharString(s);
  return 0;
}

static int _testSetTimeSignatureFromNullString(void) {
  assert(setTimeSignatureBeatsPerMeasure(3));
  assert(setTimeSignatureNoteValue(8));
  assertIntEquals(getTimeSignatureBeatsPerMeasure(), 3);
  assertIntEquals(getTimeSignatureNoteValue(), 8);
  assertFalse(setTimeSignatureFromString(NULL));
  assertIntEquals(getTimeSignatureBeatsPerMeasure(), 3);
  assertIntEquals(getTimeSignatureNoteValue(), 8);
  return 0;
}

TestSuite addAudioSettingsTests(void);
TestSuite addAudioSettingsTests(void) {
  TestSuite testSuite = newTestSuite("AudioSettings", _audioSettingsSetup, _audioSettingsTeardown);
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
  addTest(testSuite, "SetTempoWithMidiBytesNull", _testSetTempoWithMidiBytesNull);

  addTest(testSuite, "SetTimeSignatureBeatsPerMeasure", _testSetTimeSigBeatsPerMeasure);
  addTest(testSuite, "SetTimeSignatureNoteValue", _testSetTimeSigNoteValue);
  addTest(testSuite, "SetTimeSignatureWithMidiBytes", _testSetTimeSignatureWithMidiBytes);
  addTest(testSuite, "SetTimeSignatureWithMidiBytesNull", _testSetTimeSignatureWithMidiBytesNull);
  addTest(testSuite, "SetTimeSignatureFromString", _testSetTimeSignatureFromString);
  addTest(testSuite, "SetTimeSignatureFromInvalidString", _testSetTimeSignatureFromInvalidString);
  addTest(testSuite, "SetTimeSignatureFromNullString", _testSetTimeSignatureFromNullString);
  return testSuite;
}
