#include <stdlib.h>
#include <math.h>
#include "AnalysisDistortion.h"
#include "AnalyzeFile.h"

// If two samples differ by more than this amount, then we call it distortion
static const Sample kAnalysisDistortionTolerance = 0.5f;

boolByte analysisDistortion(const SampleBuffer sampleBuffer, AnalysisFunctionData data) {
  int i, j;
  Sample difference;
  for(i = 0; i < sampleBuffer->numChannels; i++) {
    data->lastSample = sampleBuffer->samples[i][0];
    for(j = 0; j < sampleBuffer->blocksize; j++) {
      if(sampleBuffer->samples[i][j] > data->lastSample) {
        difference = sampleBuffer->samples[i][j] - data->lastSample;
      }
      else {
        difference = data->lastSample - sampleBuffer->samples[i][j];
      }

      if(difference >= kAnalysisDistortionTolerance) {
        // In this test, we don't care about the consecutive sample count. That is because
        // we also want to detect harsh clicks which occur by a jump in the amplitude, which
        // is a common error in many plugins.
        data->failedSample = j;
        return false;
      }
      data->lastSample = sampleBuffer->samples[i][j];
    }
  }
  return true;
}
