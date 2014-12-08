#include "unit/TestRunner.h"
#include "io/SampleSource.h"
#include "audio/AudioSettings.h"

const char* TEST_SAMPLESOURCE_FILENAME = "test.pcm";

static void _sampleSourceSetup(void) {
  initAudioSettings();
}

static void _sampleSourceTeardown(void) {
  freeAudioSettings();
}

static int _testGuessSampleSourceTypePcm(void) {
  CharString c = newCharStringWithCString(TEST_SAMPLESOURCE_FILENAME);
  SampleSource s = sampleSourceFactory(c);
  assertIntEquals(SAMPLE_SOURCE_TYPE_PCM, s->sampleSourceType);
  freeSampleSource(s);
  freeCharString(c);
  return 0;
}

static int _testGuessSampleSourceTypeEmpty(void) {
  CharString empty = newCharString();
  SampleSource s = sampleSourceFactory(empty);
  assertIntEquals(SAMPLE_SOURCE_TYPE_SILENCE, s->sampleSourceType);
  freeSampleSource(s);
  freeCharString(empty);
  return 0;
}

static int _testGuessSampleSourceTypeWrongCase(void) {
  CharString c = newCharStringWithCString("TEST.PCM");
  SampleSource s = sampleSourceFactory(c);
  assertIntEquals(SAMPLE_SOURCE_TYPE_PCM, s->sampleSourceType);
  freeSampleSource(s);
  freeCharString(c);
  return 0;
}

TestSuite addSampleSourceTests(void);
TestSuite addSampleSourceTests(void) {
  TestSuite testSuite = newTestSuite("SampleSource", _sampleSourceSetup, _sampleSourceTeardown);
  addTest(testSuite, "GuessSampleSourceTypePcm", _testGuessSampleSourceTypePcm);
  addTest(testSuite, "GuessSampleSourceTypeEmpty", _testGuessSampleSourceTypeEmpty);
  addTest(testSuite, "GuessSampleSourceTypeWrongCase", _testGuessSampleSourceTypeWrongCase);
  return testSuite;
}
