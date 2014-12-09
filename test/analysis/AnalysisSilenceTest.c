#include "AnalysisSilence.h"
#include "unit/TestRunner.h"

static int _testAnalysisSilence(void)
{
    SampleBuffer s = newSampleBuffer(1, 64);
    AnalysisFunctionData d = newAnalysisFunctionData();
    assertFalse(analysisSilence(s, d));
    freeAnalysisFunctionData(d);
    freeSampleBuffer(s);
    return 0;
}

static int _testAnalysisNotSilence(void)
{
    SampleBuffer s = newSampleBuffer(2, 64);
    AnalysisFunctionData d = newAnalysisFunctionData();
    unsigned long i;
    unsigned int j;

    for (i = 0; i < s->blocksize; i++) {
        for (j = 0; j < s->numChannels; j++) {
            s->samples[j][i] = 32767.0;
        }
    }

    assert(analysisSilence(s, d));
    freeAnalysisFunctionData(d);
    freeSampleBuffer(s);
    return 0;
}

static int _testAnalysisNotSilenceInOneChannel(void)
{
    SampleBuffer s = newSampleBuffer(1, 64);
    AnalysisFunctionData d = newAnalysisFunctionData();
    unsigned long i;

    for (i = 0; i < s->blocksize; i++) {
        s->samples[0][i] = 32767.0;
    }

    assert(analysisSilence(s, d));
    freeAnalysisFunctionData(d);
    freeSampleBuffer(s);
    return 0;
}

TestSuite addAnalysisSilenceTests(void);
TestSuite addAnalysisSilenceTests(void)
{
    TestSuite testSuite = newTestSuite("AnalysisSilence", NULL, NULL);
    addTest(testSuite, "AnalysisSilence", _testAnalysisSilence);
    addTest(testSuite, "AnalysisNotSilence", _testAnalysisNotSilence);
    addTest(testSuite, "AnalysisNotSilenceInOneChannel", _testAnalysisNotSilenceInOneChannel);
    return testSuite;
}
