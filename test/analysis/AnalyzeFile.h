#include <stdio.h>
#include "base/Types.h"
#include "audio/SampleBuffer.h"
#include "base/CharString.h"

#ifndef MrsWatson_AnalyzeFile_h
#define MrsWatson_AnalyzeFile_h

typedef struct {
  const char* analysisName;
  void* functionPtr;
  int consecutiveFailCounter;
  Sample lastSample;
  int failedSample;
  int failTolerance;
} AnalysisFunctionDataMembers;
typedef AnalysisFunctionDataMembers* AnalysisFunctionData;
typedef boolByte (*AnalysisFuncPtr)(const SampleBuffer sampleBuffer, AnalysisFunctionData data);

typedef struct {
  CharString failedAnalysisFunctionName;
  unsigned long *failedAnalysisSample;
  unsigned long *currentBlockSample;
  boolByte *result;
  SampleBuffer sampleBuffer;
  AnalysisFunctionData functionData;
} AnalysisDataMembers;
typedef AnalysisDataMembers* AnalysisData;

AnalysisFunctionData newAnalysisFunctionData(void);
boolByte analyzeFile(const char* filename, CharString failedAnalysisFunctionName, unsigned long *failedAnalysisSample);
void freeAnalysisFunctionData(AnalysisFunctionData self);

#endif
