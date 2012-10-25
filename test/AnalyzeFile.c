#include <stdlib.h>
#include "AnalyzeFile.h"
#include "SampleSource.h"
#include "AudioSettings.h"
#include "AnalysisClipping.h"
#include "AnalysisDistortion.h"
#include "AnalysisSilence.h"
#include "LinkedList.h"

static const int kNumAnalysisFunctions = 3;

AnalysisFunctionData newAnalysisFunctionData(void) {
  AnalysisFunctionData result = (AnalysisFunctionData)malloc(sizeof(AnalysisFunctionDataMembers));
  result->analysisName = NULL;
  result->consecutiveFailCounter = 0;
  result->failedSample = 0;
  result->functionPtr = NULL;
  result->lastSample = 0;
  return result;
}

static void _setupAnalysisFunctions(LinkedList functionsList) {
  AnalysisFunctionData data;

  data = newAnalysisFunctionData();
  data->analysisName = "clipping";
  data->functionPtr = analysisClipping;
  data->consecutiveFailCounter = 0;
  data->lastSample = 0.0f;
  data->failedSample = 0;
  appendItemToList(functionsList, data);

  data = newAnalysisFunctionData();
  data->analysisName = "distortion";
  data->functionPtr = analysisDistortion;
  data->consecutiveFailCounter = 0;
  data->lastSample = 0.0f;
  data->failedSample = 0;
  appendItemToList(functionsList, data);

  data = newAnalysisFunctionData();
  data->analysisName = "silence";
  data->functionPtr = analysisSilence;
  data->consecutiveFailCounter = 0;
  data->lastSample = 0.0f;
  data->failedSample = 0;
  appendItemToList(functionsList, data);
}

static void _runAnalysisFunction(void* item, void* userData) {
  AnalysisFunctionData functionData = (AnalysisFunctionData)item;
  AnalysisFuncPtr analysisFuncPtr = (AnalysisFuncPtr)(functionData->functionPtr);
  AnalysisData analysisData = (AnalysisData)userData;

  if(!analysisFuncPtr(analysisData->sampleBuffer, functionData)) {
    copyToCharString(analysisData->failedAnalysisFunctionName, functionData->analysisName);
    *(analysisData->failedAnalysisSample) = *(analysisData->currentBlockSample) + functionData->failedSample;
    *(analysisData->result) = false;
  }
}

boolByte analyzeFile(const char* filename, CharString failedAnalysisFunctionName, unsigned long *failedAnalysisSample) {
  boolByte result;
  CharString analysisFilename;
  SampleSource sampleSource;
  LinkedList analysisFunctions = newLinkedList();
  AnalysisData analysisData;
  unsigned long currentBlockSample = 0;

  analysisData = (AnalysisData)malloc(sizeof(AnalysisDataMembers));

  // Needed to initialize new sample sources
  initAudioSettings();
  _setupAnalysisFunctions(analysisFunctions);
  analysisFilename = newCharStringWithCString(filename);
  sampleSource = newSampleSource(SAMPLE_SOURCE_TYPE_PCM, analysisFilename);

  if(sampleSource == NULL) {
    return false;
  }
  result = sampleSource->openSampleSource(sampleSource, SAMPLE_SOURCE_OPEN_READ);
  if(!result) {
    return result;
  }

  analysisData->failedAnalysisFunctionName = failedAnalysisFunctionName;
  analysisData->failedAnalysisSample = failedAnalysisSample;
  analysisData->sampleBuffer = newSampleBuffer(DEFAULT_NUM_CHANNELS, DEFAULT_BLOCKSIZE);
  analysisData->currentBlockSample = &currentBlockSample;
  analysisData->result = &result;

  while(sampleSource->readSampleBlock(sampleSource, analysisData->sampleBuffer) && result) {
    foreachItemInList(analysisFunctions, _runAnalysisFunction, analysisData);
    currentBlockSample += DEFAULT_BLOCKSIZE;
  }

  freeSampleSource(sampleSource);
  freeCharString(analysisFilename);
  freeAudioSettings();
  freeSampleBuffer(analysisData->sampleBuffer);
  free(analysisData);
  return result;
}
