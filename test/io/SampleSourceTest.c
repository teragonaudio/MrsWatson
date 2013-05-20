#include "unit/TestRunner.h"
#include "io/SampleSource.h"

const char* TEST_SAMPLESOURCE_FILENAME = "test.pcm";

static int _testGuessSampleSourceTypePcm(void) {
  assertIntEquals(sampleSourceGuess(newCharStringWithCString(TEST_SAMPLESOURCE_FILENAME)),
    SAMPLE_SOURCE_TYPE_PCM);
  return 0;
}

static int _testGuessSampleSourceTypeEmpty(void) {
  assertIntEquals(sampleSourceGuess(newCharString()), SAMPLE_SOURCE_TYPE_INVALID);
  return 0;
}

static int _testGuessSampleSourceTypeInvalid(void) {
  assertIntEquals(sampleSourceGuess(newCharString()), SAMPLE_SOURCE_TYPE_INVALID);
  return 0;
}

static int _testGuessSampleSourceTypeWrongCase(void) {
  assertIntEquals(sampleSourceGuess(newCharStringWithCString("TEST.PCM")),
    SAMPLE_SOURCE_TYPE_PCM);
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
