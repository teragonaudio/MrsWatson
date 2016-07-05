//
// AnalyzeFile.c - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include "AnalyzeFile.h"

#include "AnalysisClipping.h"
#include "AnalysisDistortion.h"
#include "AnalysisSilence.h"
#include "audio/AudioSettings.h"
#include "io/SampleSource.h"

#include <stdlib.h>

static LinkedList _getAnalysisFunctions(void) {
  AnalysisFunctionData data;
  LinkedList functionsList = newLinkedList();

  data = newAnalysisFunctionData();
  data->analysisName = "clipping";
  data->functionPtr = (void *)analysisClipping;
  data->failTolerance = kAnalysisClippingFailTolerance;
  linkedListAppend(functionsList, data);

  data = newAnalysisFunctionData();
  data->analysisName = "distortion";
  data->functionPtr = (void *)analysisDistortion;
  linkedListAppend(functionsList, data);

  data = newAnalysisFunctionData();
  data->analysisName = "silence";
  data->functionPtr = (void *)analysisSilence;
  // Use a fail tolerance here of the blocksize * 2 in order to avoid false
  // positives, which may occur with a partial last block or MIDI tests where
  // there is some silence expected between the notes.
  data->failTolerance = (int)getBlocksize() * 2;
  linkedListAppend(functionsList, data);

  return functionsList;
}

static void _runAnalysisFunction(void *item, void *userData) {
  AnalysisFunctionData functionData = (AnalysisFunctionData)item;
  AnalysisFuncPtr analysisFuncPtr =
      (AnalysisFuncPtr)(functionData->functionPtr);
  AnalysisData analysisData = (AnalysisData)userData;

  // If we are on the first block, then set the last sample value to be the same
  // as
  // the first sample in the block. Otherwise, checks which use the lastSample
  // value
  // (such as the distortion check) will erroneously fail on the first sample.
  if (*(analysisData->currentFrame) == 0) {
    for (ChannelCount i = 0; i < analysisData->sampleBuffer->numChannels; ++i) {
      functionData->lastSample[i] = analysisData->sampleBuffer->samples[i][0];
    }
  }

  if (!analysisFuncPtr(analysisData->sampleBuffer, functionData)) {
    charStringCopyCString(analysisData->failedAnalysisFunctionName,
                          functionData->analysisName);
    *(analysisData->failedAnalysisFrame) =
        *(analysisData->currentFrame) + functionData->failedSample;
    *(analysisData->failedAnalysisChannel) = functionData->failedChannel;
    *(analysisData->result) = false;
  }
}

boolByte analyzeFile(const char *filename,
                     CharString failedAnalysisFunctionName,
                     ChannelCount *failedAnalysisChannel,
                     SampleCount *failedAnalysisFrame) {
  boolByte result;
  CharString analysisFilename;
  SampleSource sampleSource;
  LinkedList analysisFunctions;
  AnalysisData analysisData = (AnalysisData)malloc(sizeof(AnalysisDataMembers));
  SampleCount currentFrame = 0;

  // Needed to initialize new sample sources
  initAudioSettings();
  analysisFunctions = _getAnalysisFunctions();
  analysisFilename = newCharStringWithCString(filename);

  sampleSource = sampleSourceFactory(analysisFilename);

  if (sampleSource == NULL) {
    freeCharString(analysisFilename);
    free(analysisData);
    freeAudioSettings();
    return false;
  }

  result =
      sampleSource->openSampleSource(sampleSource, SAMPLE_SOURCE_OPEN_READ);

  if (!result) {
    free(analysisData);
    return result;
  }

  analysisData->failedAnalysisFunctionName = failedAnalysisFunctionName;
  analysisData->failedAnalysisChannel = failedAnalysisChannel;
  analysisData->failedAnalysisFrame = failedAnalysisFrame;
  analysisData->sampleBuffer =
      newSampleBuffer(getNumChannels(), getBlocksize());
  analysisData->currentFrame = &currentFrame;
  analysisData->result = &result;

  while (
      sampleSource->readSampleBlock(sampleSource, analysisData->sampleBuffer) &&
      result) {
    linkedListForeach(analysisFunctions, _runAnalysisFunction, analysisData);
    currentFrame += getBlocksize();
  }

  sampleSource->closeSampleSource(sampleSource);
  freeSampleSource(sampleSource);
  freeCharString(analysisFilename);
  freeAudioSettings();
  freeSampleBuffer(analysisData->sampleBuffer);
  freeLinkedListAndItems(analysisFunctions,
                         (LinkedListFreeItemFunc)freeAnalysisFunctionData);
  free(analysisData);
  return result;
}

void freeAnalysisFunctionData(AnalysisFunctionData self) {
  free(self->lastSample);
  free(self);
}

AnalysisFunctionData newAnalysisFunctionData(void) {
  AnalysisFunctionData result =
      (AnalysisFunctionData)malloc(sizeof(AnalysisFunctionDataMembers));
  result->analysisName = NULL;
  result->consecutiveFailCounter = 0;
  result->failedSample = 0;
  result->functionPtr = NULL;
  // TODO: Should use max channels, when we get that
  result->lastSample = (Sample *)malloc(sizeof(Sample) * 2);
  result->lastSample[0] = 0.0f;
  result->lastSample[1] = 0.0f;
  result->failTolerance = 0;
  return result;
}
