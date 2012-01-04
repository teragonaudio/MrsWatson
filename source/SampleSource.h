//
//  SampleSource.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "Types.h"
#include "SampleBuffer.h"
#include "CharString.h"

#ifndef MrsWatson_SampleSource_h
#define MrsWatson_SampleSource_h

typedef enum {
  SAMPLE_SOURCE_TYPE_INVALID,
  SAMPLE_SOURCE_TYPE_PCM_FILE,
  SAMPLE_SOURCE_TYPE_PCM_STREAM,
} SampleSourceType;

typedef boolean (*OpenSampleSourceFunc)(void*);
typedef boolean (*ReadSampleBlockFunc)(void*, SampleBuffer);
typedef void (*FreeSampleSourceDataFunc)(void*);

typedef struct {
  SampleSourceType sampleSourceType;
  CharString sourceName;
  int numChannels;
  float sampleRate;

  OpenSampleSourceFunc openSampleSource;
  ReadSampleBlockFunc readSampleBlock;
  FreeSampleSourceDataFunc freeSampleSourceData;

  void* extraData;
} SampleSourceMembers;

typedef SampleSourceMembers* SampleSource;

SampleSourceType guessSampleSourceType(CharString sampleSourceTypeString);
SampleSource newSampleSource(SampleSourceType sampleSourceType, const CharString sampleSourceName);
void freeSampleSource(SampleSource sampleSource);

#endif
