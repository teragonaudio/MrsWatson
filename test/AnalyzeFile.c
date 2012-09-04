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
  result[0]->analysisName = "Clipping";
  result[0]->functionPtr = analysisClipping;
  result[0]->consecutiveFailCounter = 0;
  result[0]->lastSample = 0.0f;

  result[1] = malloc(sizeof(AnalysisFunctionData));
  result[1]->analysisName = "Distortion";
  result[1]->functionPtr = analysisDistortion;
  result[1]->consecutiveFailCounter = 0;
  result[1]->lastSample = 0.0f;

  result[2] = malloc(sizeof(AnalysisFunctionData));
  result[2]->analysisName = "Silence";
  result[2]->functionPtr = analysisSilence;
  result[2]->consecutiveFailCounter = 0;
  result[2]->lastSample = 0.0f;

  return result;
}

boolByte analyzeFile(const char* filename) {
  boolByte result;
  CharString analysisFilename;
  SampleBuffer sampleBuffer;
  SampleSource sampleSource;
  AnalysisFuncPtr analysisFuncPtr;
  AnalysisFunctionData* analysisFunctions;
  int i;

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
  while(sampleSource->readSampleBlock(sampleSource, sampleBuffer)) {
    for(i = 0; i < kNumAnalysisFunctions; i++) {
      analysisFuncPtr = (AnalysisFuncPtr)(analysisFunctions[i]->functionPtr);
      if(!analysisFuncPtr(sampleBuffer, analysisFunctions[i])) {
        result |= false;
      }
    }
  }

  freeSampleBuffer(sampleBuffer);
  freeSampleSource(sampleSource);
  freeCharString(analysisFilename);
  freeAudioSettings();
  return result;
}
