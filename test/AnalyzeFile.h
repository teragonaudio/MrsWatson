#include <stdio.h>
#include "Types.h"
#include "SampleBuffer.h"

#ifndef MrsWatson_AnalyzeFile_h
#define MrsWatson_AnalyzeFile_h

// Number of consecutive samples which need to fail in order for the test to fail
static const int kAnalysisDefaultFailTolerance = 4;

typedef struct {
  const char* analysisName;
  void* functionPtr;
  int consecutiveFailCounter;
  Sample lastSample;
} AnalysisFunctionDataMembers;

typedef AnalysisFunctionDataMembers* AnalysisFunctionData;
typedef boolByte (*AnalysisFuncPtr)(const SampleBuffer sampleBuffer, AnalysisFunctionData data);

boolByte analyzeFile(const char* filename);

#endif
