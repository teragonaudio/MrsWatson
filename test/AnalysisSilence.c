#include "AnalysisSilence.h"
#include "AnalyzeFile.h"

boolByte analysisSilence(const SampleBuffer sampleBuffer, AnalysisFunctionData data) {
  int i, j;
  for(i = 0; i < sampleBuffer->numChannels; i++) {
    for(j = 0; j < sampleBuffer->blocksize; j++) {
      if(sampleBuffer->samples[i][j] == 0.0f) {
        data->consecutiveFailCounter++;
        if(data->consecutiveFailCounter > kAnalysisDefaultFailTolerance) {
          data->failedSample = j;
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