#include "TestRunner.h"
#include "SampleSource.h"

static int _testGuessSampleSourceTypePcm(void) {
  return 0;
}

static int _testGuessSampleSourceTypeNull(void) {
  return 0;
}

static int _testGuessSampleSourceTypeEmpty(void) {
  return 0;
}

static int _testGuessSampleSourceTypeInvalid(void) {
  return 0;
}

static int _testGuessSampleSourceTypeWrongCase(void) {
  return 0;
}

TestSuite addSampleSourceTests(void);
TestSuite addSampleSourceTests(void) {
  TestSuite testSuite = newTestSuite("SampleSource", NULL, NULL);
  addTest(testSuite, "GuessSampleSourceTypePcm", NULL); // _testGuessSampleSourceTypePcm);
  addTest(testSuite, "GuessSampleSourceTypeNull", NULL); // _testGuessSampleSourceTypeNull);
  addTest(testSuite, "GuessSampleSourceTypeEmpty", NULL); // _testGuessSampleSourceTypeEmpty);
  addTest(testSuite, "GuessSampleSourceTypeInvalid", NULL); // _testGuessSampleSourceTypeInvalid);
  addTest(testSuite, "GuessSampleSourceTypeWrongCase", NULL); // _testGuessSampleSourceTypeWrongCase);
  return testSuite;
}
