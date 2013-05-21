#include "unit/TestRunner.h"
#include "io/SampleSource.h"

const char* TEST_SAMPLESOURCE_FILENAME = "test.pcm";

static int _testGuessSampleSourceTypePcm(void) {
  CharString c = newCharStringWithCString(TEST_SAMPLESOURCE_FILENAME);
  assertIntEquals(sampleSourceGuess(c), SAMPLE_SOURCE_TYPE_PCM);
  freeCharString(c);
  return 0;
}

static int _testGuessSampleSourceTypeEmpty(void) {
  CharString empty = newCharString();
  assertIntEquals(sampleSourceGuess(empty), SAMPLE_SOURCE_TYPE_INVALID);
  freeCharString(empty);
  return 0;
}

static int _testGuessSampleSourceTypeInvalid(void) {
  CharString empty = newCharString();
  assertIntEquals(sampleSourceGuess(empty), SAMPLE_SOURCE_TYPE_INVALID);
  freeCharString(empty);
  return 0;
}

static int _testGuessSampleSourceTypeWrongCase(void) {
  CharString c = newCharStringWithCString("TEST.PCM");
  assertIntEquals(sampleSourceGuess(c), SAMPLE_SOURCE_TYPE_PCM);
  freeCharString(c);
  return 0;
}

TestSuite addSampleSourceTests(void);
TestSuite addSampleSourceTests(void) {
  TestSuite testSuite = newTestSuite("SampleSource", NULL, NULL);
  addTest(testSuite, "GuessSampleSourceTypePcm", _testGuessSampleSourceTypePcm);
  addTest(testSuite, "GuessSampleSourceTypeEmpty", _testGuessSampleSourceTypeEmpty);
  addTest(testSuite, "GuessSampleSourceTypeInvalid", _testGuessSampleSourceTypeInvalid);
  addTest(testSuite, "GuessSampleSourceTypeWrongCase", _testGuessSampleSourceTypeWrongCase);
  return testSuite;
}
