//
//  InputSourcePcmFile.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "InputSourcePcmFile.h"

static bool _openInputSourcePcmFile(void* inputSourcePtr, const CharString filename) {
  InputSource inputSource = inputSourcePtr;
  strncpy(inputSource->inputSourceName, filename, STRING_LENGTH);

  InputSourcePcmFileData extraData = inputSource->extraData;
  extraData->dataBufferNumItems = 0;
  extraData->fileHandle = fopen(filename, "rb");
  if(extraData == NULL) {
    // TODO: Error
    return false;
  }

  return true;
}

static void _convertPcmDataToFloat(const short* inPcmData, Sample* outFloatData, const long numSamples) {
  for(long i = 0; i < numSamples; i++) {
    outFloatData[i] = (Sample)inPcmData[i] / 32768.0f;

    // Apply brickwall limiter to prevent clipping
    if(outFloatData[i] > 1.0f) {
      outFloatData[i] = 1.0f;
    }
    else if(outFloatData[i] < -1.0f) {
      outFloatData[i] = -1.0f;
    }
  }
}

static bool _readBlockPcmFile(void* inputSourcePtr, SampleBuffer sampleBuffer) {
  InputSource inputSource = inputSourcePtr;
  InputSourcePcmFileData extraData = inputSource->extraData;
  if(extraData->dataBufferNumItems == 0) {
    extraData->dataBufferNumItems = (size_t)(sampleBuffer->numChannels * sampleBuffer->blocksize);
    extraData->interlacedPcmDataBuffer = malloc(sizeof(short) * extraData->dataBufferNumItems);
    extraData->interlacedSampleBuffer = malloc(sizeof(Sample) * extraData->dataBufferNumItems);
  }

  // Clear the PCM data buffer, or else the last block will have dirty samples in the end
  memset(extraData->interlacedPcmDataBuffer, 0, sizeof(short) * extraData->dataBufferNumItems);

  bool result = true;
  size_t bytesRead = fread(extraData->interlacedPcmDataBuffer, sizeof(short), extraData->dataBufferNumItems, extraData->fileHandle);
  if(bytesRead < extraData->dataBufferNumItems * sizeof(short)) {
    // TODO: End of file reached -- do something special?
    result = false;
  }

  // Convert integer PCM data to floating point
  _convertPcmDataToFloat(extraData->interlacedPcmDataBuffer, extraData->interlacedSampleBuffer, extraData->dataBufferNumItems);
  copyInterlacedSamplesToSampleBuffer(extraData->interlacedSampleBuffer, sampleBuffer);

  return result;
}

static void _freeInputSourceDataPcmFile(void* inputSourceDataPtr) {
  InputSourcePcmFileData extraData = inputSourceDataPtr;
  free(extraData->interlacedPcmDataBuffer);
  free(extraData->interlacedSampleBuffer);
  if(extraData->fileHandle != NULL) {
    fclose(extraData->fileHandle);
  }
  free(extraData);
}

InputSource newInputSourcePcmFile() {
  InputSource inputSource = malloc(sizeof(InputSourceMembers));

  inputSource->inputSourceType = INPUT_SOURCE_TYPE_PCM_FILE;
  inputSource->inputSourceName = newCharString();
  // TODO: Need a way to pass in channels, bitrate, sample rate
  inputSource->numChannels = 2;
  inputSource->sampleRate = 44100.0f;

  inputSource->openInputSource = _openInputSourcePcmFile;
  inputSource->readBlock = _readBlockPcmFile;
  inputSource->freeInputSourceData = _freeInputSourceDataPcmFile;

  inputSource->extraData = malloc(sizeof(InputSourcePcmFileDataMembers));

  return inputSource;
}
