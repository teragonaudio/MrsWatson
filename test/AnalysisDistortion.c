#include <stdlib.h>
#include "AnalysisDistortion.h"

// If two samples differ by more than this amount, then we call it distortion
static const int kAnalysisDistortionTolerance = 200;

boolByte analysisDistortion(const SampleBuffer sampleBuffer, AnalysisFunctionData data) {
  int i, j;
  for(i = 0; i < sampleBuffer->numChannels; i++) {
    for(j = 0; j < sampleBuffer->blocksize; j++) {
      if(abs((int)sampleBuffer->samples[i][j]) - abs((int)data->lastSample) >= kAnalysisDistortionTolerance) {
        // In this test, we don't care about the consecutive sample count. That is because
        // we also want to detect harsh clicks which occur by a jump in the amplitude, which
        // is a common error in many plugins.
        return false;
      }
    }
  }
  return true;
}
