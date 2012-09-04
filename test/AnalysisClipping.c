#include <stdlib.h>
#include "AnalysisClipping.h"

boolByte analysisClipping(const SampleBuffer sampleBuffer, AnalysisFunctionData data) {
  int i, j;
  for(i = 0; i < sampleBuffer->numChannels; i++) {
    for(j = 0; j < sampleBuffer->blocksize; j++) {
      if(abs((int)sampleBuffer->samples[i][j]) >= 32767) {
        if(data->consecutiveFailCounter > kAnalysisDefaultFailTolerance) {
          return false;
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