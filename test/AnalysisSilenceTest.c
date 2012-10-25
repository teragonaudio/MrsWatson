#include "AnalysisSilence.h"
#include "TestRunner.h"

static int _testAnalysisSilence(void) {
  SampleBuffer s = newSampleBuffer(1, 8);
  AnalysisFunctionData d = newAnalysisFunctionData();
  assertFalse(analysisSilence(s, d));
  return 0;
}

static int _testAnalysisNotSilence(void) {
  SampleBuffer s = newSampleBuffer(1, 8);
  AnalysisFunctionData d = newAnalysisFunctionData();
  int i;
  for(i = 0; i < s->blocksize; i++) {
    s->samples[0][i] = 32767;
  }
  assert(analysisSilence(s, d));
  return 0;
}

TestSuite addAnalysisSilenceTests(void);
TestSuite addAnalysisSilenceTests(void) {
  TestSuite testSuite = newTestSuite("AnalysisSilence", NULL, NULL);
  addTest(testSuite, "AnalysisSilence", _testAnalysisSilence);
  addTest(testSuite, "AnalysisNotSilence", _testAnalysisNotSilence);
  return testSuite;
}
