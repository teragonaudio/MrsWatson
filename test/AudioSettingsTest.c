#include "TestRunner.h"
#include "AudioSettings.h"

static void _audioSettingsSetup(void) {
  initAudioSettings();
}

static void _audioSettingsTeardown(void) {
  freeAudioSettings();
}

static int _testInitAudioSettings(void) {
  _assertDoubleEquals(getSampleRate(), DEFAULT_SAMPLE_RATE);
  _assertIntEquals(getNumChannels(), DEFAULT_NUM_CHANNELS);
  _assertIntEquals(getBlocksize(), DEFAULT_BLOCKSIZE);
  _assertDoubleEquals(getTempo(), DEFAULT_TEMPO);
  _assertIntEquals(getTimeSignatureBeatsPerMeasure(), DEFAULT_TIMESIG_BEATS_PER_MEASURE);
  _assertIntEquals(getTimeSignatureNoteValue(), DEFAULT_TIMESIG_NOTE_VALUE);
  return 0;
}

static int _testSetSampleRate(void) {
  setSampleRate(22050.0);
  _assertDoubleEquals(getSampleRate(), 22050.0);
  return 0;
}

static int _testSetNumChannels(void) {
  setNumChannels(4);
  _assertIntEquals(getNumChannels(), 4);
  return 0;
}

static int _testSetBlocksize(void) {
  setBlocksize(123);
  _assertIntEquals(getBlocksize(), 123);
  return 0;
}

static int _testSetTempo(void) {
  setTempo(123.45);
  _assertDoubleEquals(getTempo(), 123.45);
  return 0;
}

static int _testSetTimeSigBeatsPerMeasure(void) {
  setTimeSignatureBeatsPerMeasure(8);
  _assertIntEquals(getTimeSignatureBeatsPerMeasure(), 8);
  return 0;
}

static int _testSetTimeSigNoteValue(void) {
  setTimeSignatureNoteValue(2);
  _assertIntEquals(getTimeSignatureNoteValue(), 2);
  return 0;
}

void runAudioSettingsTests(void);
void runAudioSettingsTests(void) {
  _startTestSection();
  _runTest("Initialization", _testInitAudioSettings, _audioSettingsSetup, _audioSettingsTeardown);
  _runTest("Set sample rate", _testSetSampleRate, _audioSettingsSetup, _audioSettingsTeardown);
  _runTest("Set num channels", _testSetNumChannels, _audioSettingsSetup, _audioSettingsTeardown);
  _runTest("Set blocksize", _testSetBlocksize, _audioSettingsSetup, _audioSettingsTeardown);
  _runTest("Set tempo", _testSetTempo, _audioSettingsSetup, _audioSettingsTeardown);
  _runTest("Set time signature beats per measure", _testSetTimeSigBeatsPerMeasure, _audioSettingsSetup, _audioSettingsTeardown);
  _runTest("Set time signature note value", _testSetTimeSigNoteValue, _audioSettingsSetup, _audioSettingsTeardown);
}