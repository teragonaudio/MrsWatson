#include "TestRunner.h"
#include "SampleBuffer.h"

static int _testNewSampleBuffer(void) {
  return 0;
}

static int _testClearSampleBuffer(void) {
  return 0;
}

static int _testCopySampleBuffers(void) {
  return 0;
}

static int _testCopySampleBuffersDifferentSizes(void) {
  return 0;
}

TestSuite addSampleBufferTests(void);
TestSuite addSampleBufferTests(void) {
  TestSuite testSuite = newTestSuite("SampleBuffer", NULL, NULL);
  addTest(testSuite, "NewObject", NULL); // _testNewSampleBuffer);
  addTest(testSuite, "ClearSampleBuffer", NULL); // _testClearSampleBuffer);
  addTest(testSuite, "CopySampleBuffers", NULL); // _testCopySampleBuffers);
  addTest(testSuite, "CopySampleBuffersDifferentSizes", NULL); // _testCopySampleBuffersDifferentSizes);
  return testSuite;
}
