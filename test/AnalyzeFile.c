#include <stdlib.h>
#include "AnalyzeFile.h"
#include "SampleSource.h"
#include "AudioSettings.h"
#include "AnalysisClipping.h"
#include "AnalysisDistortion.h"
#include "AnalysisSilence.h"

static const int kNumAnalysisFunctions = 3;

static AnalysisFunctionData* _setupAnalysisFunctions(void) {
  AnalysisFunctionData* result = malloc(sizeof(AnalysisFunctionData*) * kNumAnalysisFunctions);

  result[0] = malloc(sizeof(AnalysisFunctionData));
  result[0]->analysisName = "clipping";
  result[0]->functionPtr = analysisClipping;
  result[0]->consecutiveFailCounter = 0;
  result[0]->lastSample = 0.0f;
  result[0]->failedSample = 0;

  result[1] = malloc(sizeof(AnalysisFunctionData));
  result[1]->analysisName = "distortion";
  result[1]->functionPtr = analysisDistortion;
  result[1]->consecutiveFailCounter = 0;
  result[1]->lastSample = 0.0f;
  result[1]->failedSample = 0;

  result[2] = malloc(sizeof(AnalysisFunctionData));
  result[2]->analysisName = "silence";
  result[2]->functionPtr = analysisSilence;
  result[2]->consecutiveFailCounter = 0;
  result[2]->lastSample = 0.0f;
  result[2]->failedSample = 0;

  return result;
}

boolByte analyzeFile(const char* filename, CharString failedAnalysisFunctionName, unsigned long *failedAnalysisSample) {
  boolByte result;
  CharString analysisFilename;
  SampleBuffer sampleBuffer;
  SampleSource sampleSource;
  AnalysisFuncPtr analysisFuncPtr;
  AnalysisFunctionData* analysisFunctions;
  unsigned long currentBlockSample = 0;
  int i;

  // Needed to initialize new sample sources
  initAudioSettings();
  analysisFunctions = _setupAnalysisFunctions();
  analysisFilename = newCharString();
  copyToCharString(analysisFilename, filename);
  sampleSource = newSampleSource(SAMPLE_SOURCE_TYPE_PCM, analysisFilename);
  if(sampleSource == NULL) {
    return false;
  }
  result = sampleSource->openSampleSource(sampleSource, SAMPLE_SOURCE_OPEN_READ);
  if(!result) {
    return result;
  }

  sampleBuffer = newSampleBuffer(DEFAULT_NUM_CHANNELS, DEFAULT_BLOCKSIZE);
  while(sampleSource->readSampleBlock(sampleSource, sampleBuffer) && result) {
    for(i = 0; i < kNumAnalysisFunctions; i++) {
      analysisFuncPtr = (AnalysisFuncPtr)(analysisFunctions[i]->functionPtr);
      if(!analysisFuncPtr(sampleBuffer, analysisFunctions[i])) {
        copyToCharString(failedAnalysisFunctionName, analysisFunctions[i]->analysisName);
        *failedAnalysisSample = currentBlockSample + analysisFunctions[i]->failedSample;
        result = false;
        break;
      }
    }
    currentBlockSample += DEFAULT_BLOCKSIZE;
  }

  freeSampleBuffer(sampleBuffer);
  freeSampleSource(sampleSource);
  freeCharString(analysisFilename);
  freeAudioSettings();
  return result;
}
