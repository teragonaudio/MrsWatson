#include "unit/TestRunner.h"
#include "audio/SampleBuffer.h"
#include "sequencer/AudioSettings.h"

static SampleBuffer _newMockSampleBuffer(void) {
  return newSampleBuffer(1, 1);
}

static int _testNewSampleBuffer(void) {
  SampleBuffer s = _newMockSampleBuffer();
  assertIntEquals(s->numChannels, 1);
  assertIntEquals(s->blocksize, 1);
  return 0;
}

static int _testNewSampleBufferInvalidNumChannels(void) {
  SampleBuffer s = newSampleBuffer(0, 1);
  assertIsNull(s);
  return 0;
}

static int _testNewSampleBufferInvalidSampleRate(void) {
  SampleBuffer s = newSampleBuffer(1, 0);
  assertIsNull(s);
  return 0;
}

static int _testClearSampleBuffer(void) {
  SampleBuffer s = _newMockSampleBuffer();
  s->samples[0][0] = 123;
  clearSampleBuffer(s);
  assertDoubleEquals(s->samples[0][0], 0.0, TEST_FLOAT_TOLERANCE);
  return 0;
}

static int _testCopySampleBuffers(void) {
  SampleBuffer s1 = _newMockSampleBuffer();
  SampleBuffer s2 = _newMockSampleBuffer();
  s1->samples[0][0] = 123.0;
  copySampleBuffers(s2, s1);
  assertDoubleEquals(s2->samples[0][0], 123.0, TEST_FLOAT_TOLERANCE);
  return 0;
}

static int _testCopySampleBuffersDifferentBlocksizes(void) {
  SampleBuffer s1 = newSampleBuffer(1, DEFAULT_BLOCKSIZE);
  SampleBuffer s2 = _newMockSampleBuffer();
  s1->samples[0][0] = 123.0;
  copySampleBuffers(s2, s1);
  // Contents should not change; copying with different sizes is invalid
  assertDoubleEquals(s1->samples[0][0], 123.0, TEST_FLOAT_TOLERANCE);
  return 0;
}

static int _testCopySampleBuffersDifferentChannels(void) {
  SampleBuffer s1 = newSampleBuffer(DEFAULT_NUM_CHANNELS, 1);
  SampleBuffer s2 = _newMockSampleBuffer();
  s1->samples[0][0] = 123.0;
  copySampleBuffers(s2, s1);
  // Contents should not change; copying with different sizes is invalid
  assertDoubleEquals(s1->samples[0][0], 123.0, TEST_FLOAT_TOLERANCE);
  return 0;
}

TestSuite addSampleBufferTests(void);
TestSuite addSampleBufferTests(void) {
  TestSuite testSuite = newTestSuite("SampleBuffer", NULL, NULL);
  addTest(testSuite, "NewObject", _testNewSampleBuffer);
  addTest(testSuite, "NewSampleBufferInvalidNumChannels", _testNewSampleBufferInvalidNumChannels);
  addTest(testSuite, "NewSampleBufferSampleRate", _testNewSampleBufferInvalidSampleRate);
  addTest(testSuite, "ClearSampleBuffer", _testClearSampleBuffer);
  addTest(testSuite, "CopySampleBuffers", _testCopySampleBuffers);
  addTest(testSuite, "CopySampleBuffersDifferentSizes",  _testCopySampleBuffersDifferentBlocksizes);
  addTest(testSuite, "CopySampleBuffersDifferentChannels",  _testCopySampleBuffersDifferentChannels);
  return testSuite;
}
