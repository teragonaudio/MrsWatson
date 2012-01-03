//
//  InputSource.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "Types.h"
#include "SampleBuffer.h"
#include "CharString.h"

#ifndef MrsWatson_InputSource_h
#define MrsWatson_InputSource_h

typedef enum {
  INPUT_SOURCE_TYPE_INVALID,
  INPUT_SOURCE_TYPE_PCM_FILE,
  INPUT_SOURCE_TYPE_PCM_STREAM,
  NUM_INPUT_SOURCE_TYPES
} InputSourceType;

typedef bool(*OpenInputSourceFunc)(void*, const CharString filename);
typedef bool(*ReadBlockFunc)(void*, SampleBuffer);
typedef void(*FreeInputSourceDataFunc)(void*);

typedef struct {
  InputSourceType inputSourceType;
  CharString inputSourceName;
  int numChannels;
  float sampleRate;

  OpenInputSourceFunc openInputSource;
  ReadBlockFunc readBlock;
  FreeInputSourceDataFunc freeInputSourceData;

  void* extraData;
} InputSourceMembers;

typedef InputSourceMembers* InputSource;

InputSourceType guessInputSourceType(CharString inputSourceTypeString);
InputSource newInputSource(InputSourceType inputSourceType);
void freeInputSource(InputSource inputSource);

#endif
