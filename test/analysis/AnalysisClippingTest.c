#include "unit/TestRunner.h"
#include "AnalysisClipping.h"

static int _testAnalysisClipping(void) {
  SampleBuffer s = newSampleBuffer(1, 8);
  AnalysisFunctionData d = newAnalysisFunctionData();
  int i;
  for(i = 0; i < s->blocksize; i++) {
    s->samples[0][i] = 32767;
  }
  assertFalse(analysisClipping(s, d));
  return 0;
}

static int _testAnalysisNotClipping(void) {
  SampleBuffer s = newSampleBuffer(1, 8);
  AnalysisFunctionData d = newAnalysisFunctionData();
  assert(analysisClipping(s, d));
  return 0;
}

TestSuite addAnalysisClippingTests(void);
TestSuite addAnalysisClippingTests(void) {
  TestSuite testSuite = newTestSuite("AnalysisClipping", NULL, NULL);
  addTest(testSuite, "AnalysisClipping", _testAnalysisClipping);
  addTest(testSuite, "AnalysisNotClipping", _testAnalysisNotClipping);
  return testSuite;
}
