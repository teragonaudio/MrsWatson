#include "audio/AudioSettings.h"
#include "audio/SampleBuffer.h"
#include "unit/TestRunner.h"

static SampleBuffer _newMockSampleBuffer(void) {
  return newSampleBuffer(1, 1);
}

static int _testNewSampleBuffer(void) {
  SampleBuffer s = _newMockSampleBuffer();
  assertIntEquals(s->numChannels, 1);
  assertUnsignedLongEquals(s->blocksize, 1l);
  freeSampleBuffer(s);
  return 0;
}

static int _testNewSampleBufferInvalidNumChannels(void) {
  SampleBuffer s = newSampleBuffer(0, 1);
  assertIsNull(s);
  freeSampleBuffer(s);
  return 0;
}

static int _testNewSampleBufferInvalidSampleRate(void) {
  SampleBuffer s = newSampleBuffer(1, 0);
  assertIsNull(s);
  freeSampleBuffer(s);
  return 0;
}

static int _testNewSampleBufferMultichannel(void) {
  SampleBuffer s = newSampleBuffer(8, 128);
  unsigned int i, j;
  assertNotNull(s);
  assertIntEquals(s->numChannels, 8);
  // Actually write a bunch of samples to expose memory corruption
  for(i = 0; i < s->blocksize; ++i) {
    for(j = 0; j < s->numChannels; ++j) {
      s->samples[j][i] = 0.5f;
    }
  }
  freeSampleBuffer(s);
  return 0;
}

static int _testClearSampleBuffer(void) {
  SampleBuffer s = _newMockSampleBuffer();
  s->samples[0][0] = 123;
  sampleBufferClear(s);
  assertDoubleEquals(s->samples[0][0], 0.0, TEST_FLOAT_TOLERANCE);
  freeSampleBuffer(s);
  return 0;
}

static int _testCopySampleBuffers(void) {
  SampleBuffer s1 = _newMockSampleBuffer();
  SampleBuffer s2 = _newMockSampleBuffer();
  s1->samples[0][0] = 123.0;
  assert(sampleBufferCopy(s2, s1));
  assertDoubleEquals(s2->samples[0][0], 123.0, TEST_FLOAT_TOLERANCE);
  freeSampleBuffer(s1);
  freeSampleBuffer(s2);
  return 0;
}

static int _testCopySampleBuffersDifferentBlocksizes(void) {
  SampleBuffer s1 = newSampleBuffer(1, DEFAULT_BLOCKSIZE);
  SampleBuffer s2 = _newMockSampleBuffer();

  s1->samples[0][0] = 123.0;
  assertFalse(sampleBufferCopy(s2, s1));
  // Contents should not change; copying with different sizes is invalid
  assertDoubleEquals(s1->samples[0][0], 123.0, TEST_FLOAT_TOLERANCE);

  freeSampleBuffer(s1);
  freeSampleBuffer(s2);
  return 0;
}

static int _testCopySampleBuffersDifferentChannelsBigger(void) {
  SampleBuffer s1 = newSampleBuffer(4, 1);
  SampleBuffer s2 = newSampleBuffer(2, 1);

  s2->samples[0][0] = 1.0;
  s2->samples[1][0] = 2.0;

  assert(sampleBufferCopy(s1, s2));
  assertDoubleEquals(s1->samples[0][0], 1.0, TEST_FLOAT_TOLERANCE);
  assertDoubleEquals(s1->samples[1][0], 2.0, TEST_FLOAT_TOLERANCE);
  assertDoubleEquals(s1->samples[2][0], 1.0, TEST_FLOAT_TOLERANCE);
  assertDoubleEquals(s1->samples[3][0], 2.0, TEST_FLOAT_TOLERANCE);

  freeSampleBuffer(s1);
  freeSampleBuffer(s2);
  return 0;
}

static int _testCopySampleBuffersDifferentChannelsSmaller(void) {
  SampleBuffer s1 = newSampleBuffer(1, 1);
  SampleBuffer s2 = newSampleBuffer(4, 1);
  unsigned int i;

  for(i = 0; i < s1->numChannels; i++) {
    s1->samples[i][0] = 1.0;
  }
  for(i = 0; i < s2->numChannels; i++) {
    s2->samples[i][0] = 2.0;
  }

  assert(sampleBufferCopy(s1, s2));
  assertDoubleEquals(s1->samples[0][0], 2.0, TEST_FLOAT_TOLERANCE);

  freeSampleBuffer(s1);
  freeSampleBuffer(s2);
  return 0;
}

static int _testResizeSampleBufferExpand(void) {
  SampleBuffer s = _newMockSampleBuffer();
  s->samples[0][0] = 1.0;
  assert(sampleBufferResize(s, 2, false));
  assertIntEquals(s->numChannels, 2);
  assertDoubleEquals(s->samples[0][0], 1.0, TEST_FLOAT_TOLERANCE);
  assertDoubleEquals(s->samples[1][0], 0.0, TEST_FLOAT_TOLERANCE);
  freeSampleBuffer(s);
  return 0;
}

static int _testResizeSampleBufferExpandCopy(void) {
  SampleBuffer s = _newMockSampleBuffer();
  s->samples[0][0] = 1.0;
  assert(sampleBufferResize(s, 2, true));
  assertIntEquals(s->numChannels, 2);
  assertDoubleEquals(s->samples[0][0], 1.0, TEST_FLOAT_TOLERANCE);
  assertDoubleEquals(s->samples[1][0], 1.0, TEST_FLOAT_TOLERANCE);
  freeSampleBuffer(s);
  return 0;
}

static int _testResizeSampleBufferShrink(void) {
  SampleBuffer s = newSampleBuffer(2, 1);
  s->samples[0][0] = 1.0;
  assert(sampleBufferResize(s, 1, false));
  assertIntEquals(s->numChannels, 1);
  assertDoubleEquals(s->samples[0][0], 1.0, TEST_FLOAT_TOLERANCE);
  freeSampleBuffer(s);
  return 0;
}

static int _testResizeSampleBufferInvalidSize(void) {
  SampleBuffer s = _newMockSampleBuffer();
  assertFalse(sampleBufferResize(s, 0, true));
  freeSampleBuffer(s);
  return 0;  
}

static int _testResizeSampleBufferSameSize(void) {
  SampleBuffer s = _newMockSampleBuffer();
  assertFalse(sampleBufferResize(s, s->numChannels, true));
  freeSampleBuffer(s);
  return 0;  
}

static int _testFreeNullSampleBuffer(void) {
  freeSampleBuffer(NULL);
  return 0;
}

TestSuite addSampleBufferTests(void);
TestSuite addSampleBufferTests(void) {
  TestSuite testSuite = newTestSuite("SampleBuffer", NULL, NULL);
  addTest(testSuite, "NewObject", _testNewSampleBuffer);
  addTest(testSuite, "NewSampleBufferInvalidNumChannels", _testNewSampleBufferInvalidNumChannels);
  addTest(testSuite, "NewSampleBufferSampleRate", _testNewSampleBufferInvalidSampleRate);
  addTest(testSuite, "NewSampleBufferMultichannel", _testNewSampleBufferMultichannel);
  addTest(testSuite, "ClearSampleBuffer", _testClearSampleBuffer);
  addTest(testSuite, "CopySampleBuffers", _testCopySampleBuffers);
  addTest(testSuite, "CopySampleBuffersDifferentSizes",  _testCopySampleBuffersDifferentBlocksizes);
  addTest(testSuite, "CopySampleBuffersDifferentChannelsBigger",  _testCopySampleBuffersDifferentChannelsBigger);
  addTest(testSuite, "CopySampleBuffersDifferentChannelsSmaller",  _testCopySampleBuffersDifferentChannelsSmaller);
  addTest(testSuite, "ResizeSampleBufferExpand", _testResizeSampleBufferExpand);
  addTest(testSuite, "ResizeSampleBufferExpandCopy", _testResizeSampleBufferExpandCopy);
  addTest(testSuite, "ResizeSampleBufferShrink", _testResizeSampleBufferShrink);
  addTest(testSuite, "ResizeSampleBufferInvalidSize", _testResizeSampleBufferInvalidSize);
  addTest(testSuite, "ResizeSampleBufferSameSize", _testResizeSampleBufferSameSize);
  addTest(testSuite, "FreeNullSampleBuffer", _testFreeNullSampleBuffer);
  return testSuite;
}
