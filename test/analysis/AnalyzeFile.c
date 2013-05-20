#include <stdlib.h>
#include "base/LinkedList.h"
#include "io/SampleSource.h"
#include "sequencer/AudioSettings.h"
#include "AnalysisClipping.h"
#include "AnalysisDistortion.h"
#include "AnalysisSilence.h"
#include "AnalyzeFile.h"

// Number of consecutive samples which need to fail in order for the test to fail
static const int kAnalysisDefaultFailTolerance = 16;
// Use a blocksize of the default * 2 in order to avoid false positives of the
// silence detection algorithm, since the last block is likely to be silent.
static const int kAnalysisBlocksize = DEFAULT_BLOCKSIZE * 2;

AnalysisFunctionData newAnalysisFunctionData(void) {
  AnalysisFunctionData result = (AnalysisFunctionData)malloc(sizeof(AnalysisFunctionDataMembers));
  result->analysisName = NULL;
  result->consecutiveFailCounter = 0;
  result->failedSample = 0;
  result->functionPtr = NULL;
  result->lastSample = 0.0f;
  result->failTolerance = kAnalysisDefaultFailTolerance;
  return result;
}

static void _setupAnalysisFunctions(LinkedList functionsList) {
  AnalysisFunctionData data;

  data = newAnalysisFunctionData();
  data->analysisName = "clipping";
  data->functionPtr = (void*)analysisClipping;
  linkedListAppend(functionsList, data);

  data = newAnalysisFunctionData();
  data->analysisName = "distortion";
  data->functionPtr = (void*)analysisDistortion;
  linkedListAppend(functionsList, data);

  data = newAnalysisFunctionData();
  data->analysisName = "silence";
  data->functionPtr = (void*)analysisSilence;
  data->failTolerance = kAnalysisBlocksize;
  linkedListAppend(functionsList, data);
}

static void _runAnalysisFunction(void* item, void* userData) {
  AnalysisFunctionData functionData = (AnalysisFunctionData)item;
  AnalysisFuncPtr analysisFuncPtr = (AnalysisFuncPtr)(functionData->functionPtr);
  AnalysisData analysisData = (AnalysisData)userData;

  if(!analysisFuncPtr(analysisData->sampleBuffer, functionData)) {
    charStringCopyCString(analysisData->failedAnalysisFunctionName, functionData->analysisName);
    *(analysisData->failedAnalysisSample) = *(analysisData->currentBlockSample) + functionData->failedSample;
    *(analysisData->result) = false;
  }
}

boolByte analyzeFile(const char* filename, CharString failedAnalysisFunctionName, unsigned long *failedAnalysisSample) {
  boolByte result;
  CharString analysisFilename;
  SampleSource sampleSource;
  SampleSourceType sampleSourceType;
  LinkedList analysisFunctions = newLinkedList();
  AnalysisData analysisData = (AnalysisData)malloc(sizeof(AnalysisDataMembers));
  unsigned long currentBlockSample = 0;

  // Needed to initialize new sample sources
  initAudioSettings();
  _setupAnalysisFunctions(analysisFunctions);
  analysisFilename = newCharStringWithCString(filename);
  sampleSourceType = sampleSourceGuess(analysisFilename);
  sampleSource = newSampleSource(sampleSourceType, analysisFilename);
  if(sampleSource == NULL) {
    return false;
  }
  result = sampleSource->openSampleSource(sampleSource, SAMPLE_SOURCE_OPEN_READ);
  if(!result) {
    return result;
  }

  analysisData->failedAnalysisFunctionName = failedAnalysisFunctionName;
  analysisData->failedAnalysisSample = failedAnalysisSample;
  analysisData->sampleBuffer = newSampleBuffer(DEFAULT_NUM_CHANNELS, kAnalysisBlocksize);
  analysisData->currentBlockSample = &currentBlockSample;
  analysisData->result = &result;

  while(sampleSource->readSampleBlock(sampleSource, analysisData->sampleBuffer) && result) {
    linkedListForeach(analysisFunctions, _runAnalysisFunction, analysisData);
    currentBlockSample += kAnalysisBlocksize;
  }

  freeSampleSource(sampleSource);
  freeCharString(analysisFilename);
  freeAudioSettings();
  freeSampleBuffer(analysisData->sampleBuffer);
  free(analysisData);
  return result;
}
