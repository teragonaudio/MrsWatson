#include "audio/SampleBuffer.h"
#include "base/CharString.h"
#include "base/Types.h"
#include <stdio.h>

#ifndef MrsWatson_AnalyzeFile_h
#define MrsWatson_AnalyzeFile_h

typedef struct {
  const char *analysisName;
  void *functionPtr;
  int consecutiveFailCounter;
  Sample *lastSample;
  SampleCount failedSample;
  ChannelCount failedChannel;
  int failTolerance;
} AnalysisFunctionDataMembers;
typedef AnalysisFunctionDataMembers *AnalysisFunctionData;
typedef boolByte (*AnalysisFuncPtr)(const SampleBuffer sampleBuffer,
                                    AnalysisFunctionData data);

typedef struct {
  CharString failedAnalysisFunctionName;
  SampleCount *failedAnalysisFrame;
  ChannelCount *failedAnalysisChannel;
  SampleCount *currentFrame;
  boolByte *result;
  SampleBuffer sampleBuffer;
  AnalysisFunctionData functionData;
} AnalysisDataMembers;
typedef AnalysisDataMembers *AnalysisData;

AnalysisFunctionData newAnalysisFunctionData(void);
boolByte analyzeFile(const char *filename,
                     CharString failedAnalysisFunctionName,
                     ChannelCount *failedAnalysisChannel,
                     SampleCount *failedAnalysisFrame);
void freeAnalysisFunctionData(AnalysisFunctionData self);

#endif
