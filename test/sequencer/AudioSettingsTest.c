#include "unit/TestRunner.h"
#include "sequencer/AudioSettings.h"

static void _audioSettingsSetup(void) {
  initAudioSettings();
}

static void _audioSettingsTeardown(void) {
  freeAudioSettings();
}

static int _testInitAudioSettings(void) {
  assertDoubleEquals(getSampleRate(), DEFAULT_SAMPLE_RATE, TEST_FLOAT_TOLERANCE);
  assertIntEquals(getNumChannels(), DEFAULT_NUM_CHANNELS);
  assertIntEquals(getBlocksize(), DEFAULT_BLOCKSIZE);
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

static int _testSetNumChannels(void) {
  setNumChannels(4);
  assertIntEquals(getNumChannels(), 4);
  return 0;
}

static int _testSetBlocksize(void) {
  setBlocksize(123);
  assertIntEquals(getBlocksize(), 123);
  return 0;
}

static int _testSetTempo(void) {
  setTempo(123.45);
  assertDoubleEquals(getTempo(), 123.45, TEST_FLOAT_TOLERANCE);
  return 0;
}

static int _testSetTimeSigBeatsPerMeasure(void) {
  setTimeSignatureBeatsPerMeasure(8);
  assertIntEquals(getTimeSignatureBeatsPerMeasure(), 8);
  return 0;
}

static int _testSetTimeSigNoteValue(void) {
  setTimeSignatureNoteValue(2);
  assertIntEquals(getTimeSignatureNoteValue(), 2);
  return 0;
}

TestSuite addAudioSettingsTests(void);
TestSuite addAudioSettingsTests(void) {
  TestSuite testSuite = newTestSuite("AudioSettings", _audioSettingsSetup, _audioSettingsTeardown);
  addTest(testSuite, "Initialization", _testInitAudioSettings);
  addTest(testSuite, "SetSampleRate", _testSetSampleRate);
  addTest(testSuite, "SetNumChannels", _testSetNumChannels);
  addTest(testSuite, "SetBlocksize", _testSetBlocksize);
  addTest(testSuite, "SetTempo", _testSetTempo);
  addTest(testSuite, "SetTimeSignatureBeatsPerMeasure", _testSetTimeSigBeatsPerMeasure);
  addTest(testSuite, "SetTimeSignatureNoteValue", _testSetTimeSigNoteValue);
  return testSuite;
}
