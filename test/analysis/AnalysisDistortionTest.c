#include "AnalysisDistortion.h"
#include "unit/TestRunner.h"

static int _testAnalysisDistortion(void)
{
    SampleBuffer s = newSampleBuffer(1, 8);
    AnalysisFunctionData d = newAnalysisFunctionData();
    unsigned int i;

    for (i = 0; i < s->blocksize; i++) {
        s->samples[0][i] = 0.9f * (i % 2 ? 1.0f : -1.0f);
    }

    assertFalse(analysisDistortion(s, d));
    freeAnalysisFunctionData(d);
    freeSampleBuffer(s);
    return 0;
}

static int _testAnalysisNotDistortion(void)
{
    SampleBuffer s = newSampleBuffer(1, 8);
    AnalysisFunctionData d = newAnalysisFunctionData();
    assert(analysisDistortion(s, d));
    freeAnalysisFunctionData(d);
    freeSampleBuffer(s);
    return 0;
}

TestSuite addAnalysisDistortionTests(void);
TestSuite addAnalysisDistortionTests(void)
{
    TestSuite testSuite = newTestSuite("AnalysisDistortion", NULL, NULL);
    addTest(testSuite, "AnalysisDistortion", _testAnalysisDistortion);
    addTest(testSuite, "AnalysisNotDistortion", _testAnalysisNotDistortion);
    return testSuite;
}
