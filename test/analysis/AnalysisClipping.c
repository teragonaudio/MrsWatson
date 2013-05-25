#include <stdlib.h>
#include <math.h>
#include "AnalysisClipping.h"
#include "AnalyzeFile.h"

boolByte analysisClipping(const SampleBuffer sampleBuffer, AnalysisFunctionData data) {
  unsigned int i;
  unsigned long j;
  for(i = 0; i < sampleBuffer->numChannels; i++) {
    for(j = 0; j < sampleBuffer->blocksize; j++) {
      if(fabs(sampleBuffer->samples[i][j]) >= 1.0f) {
        if(data->consecutiveFailCounter > data->failTolerance) {
          data->failedSample = j;
          return false;
        }
        else {
          data->consecutiveFailCounter++;
        }
      }
      else {
        if(data->consecutiveFailCounter > 0) {
          data->consecutiveFailCounter--;
        }
      }
    }
  }
  return true;
}
