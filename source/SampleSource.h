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
  SAMPLE_SOURCE_TYPE_PCM_STREAM, // TODO: Currently unsupported
  SAMPLE_SOURCE_TYPE_SILENCE,
  NUM_SAMPLE_SOURCES
} SampleSourceType;

typedef enum {
  SAMPLE_SOURCE_OPEN_NOT_OPENED,
  SAMPLE_SOURCE_OPEN_READ,
  SAMPLE_SOURCE_OPEN_WRITE,
  NUM_SAMPLE_SOURCE_OPEN_AS
} SampleSourceOpenAs;

typedef boolean (*OpenSampleSourceFunc)(void*, const SampleSourceOpenAs);
typedef boolean (*ReadSampleBlockFunc)(void*, SampleBuffer);
typedef boolean (*WriteSampleBlockFunc)(void*, const SampleBuffer);
typedef void (*FreeSampleSourceDataFunc)(void*);

typedef struct {
  SampleSourceType sampleSourceType;
  SampleSourceOpenAs openedAs;
  CharString sourceName;
  int numChannels;
  float sampleRate;

  OpenSampleSourceFunc openSampleSource;
  ReadSampleBlockFunc readSampleBlock;
  WriteSampleBlockFunc writeSampleBlock;
  FreeSampleSourceDataFunc freeSampleSourceData;

  void* extraData;
} SampleSourceMembers;

typedef SampleSourceMembers* SampleSource;

SampleSourceType guessSampleSourceType(CharString sampleSourceTypeString);
SampleSource newSampleSource(SampleSourceType sampleSourceType, const CharString sampleSourceName);
void freeSampleSource(SampleSource sampleSource);

#endif
