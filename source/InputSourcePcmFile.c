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

static boolean _openInputSourcePcmFile(void* inputSourcePtr) {
  InputSource inputSource = inputSourcePtr;

  InputSourcePcmFileData extraData = inputSource->extraData;
  extraData->dataBufferNumItems = 0;
  extraData->fileHandle = fopen(inputSource->inputSourceName->data, "rb");
  if(extraData == NULL) {
    // TODO: Error
    return false;
  }

  return true;
}

static void _convertPcmDataToSampleBuffer(const short* inPcmSamples, SampleBuffer sampleBuffer, const long numInterlacedSamples) {
  for(long interlacedIndex = 0, deinterlacedIndex = 0; interlacedIndex < numInterlacedSamples; interlacedIndex++) {
    for(int channelIndex = 0; channelIndex < sampleBuffer->numChannels; channelIndex++) {
      Sample convertedSample = (Sample)inPcmSamples[interlacedIndex] / 32768.0f;
      // Apply brickwall limiter to prevent clipping
      if(convertedSample > 1.0f) {
        convertedSample = 1.0f;
      }
      else if(convertedSample < -1.0f) {
        convertedSample = -1.0f;
      }
      sampleBuffer->samples[channelIndex][deinterlacedIndex] = convertedSample;
    }
    deinterlacedIndex++;
  }
}

static boolean _readBlockPcmFile(void* inputSourcePtr, SampleBuffer sampleBuffer) {
  InputSource inputSource = inputSourcePtr;
  InputSourcePcmFileData extraData = inputSource->extraData;
  if(extraData->dataBufferNumItems == 0) {
    extraData->dataBufferNumItems = (size_t)(sampleBuffer->numChannels * sampleBuffer->blocksize);
    extraData->interlacedPcmDataBuffer = malloc(sizeof(short) * extraData->dataBufferNumItems);
  }

  // Clear the PCM data buffer, or else the last block will have dirty samples in the end
  memset(extraData->interlacedPcmDataBuffer, 0, sizeof(short) * extraData->dataBufferNumItems);

  boolean result = true;
  size_t bytesRead = fread(extraData->interlacedPcmDataBuffer, sizeof(short), extraData->dataBufferNumItems, extraData->fileHandle);
  if(bytesRead < extraData->dataBufferNumItems * sizeof(short)) {
    // TODO: End of file reached -- do something special?
    result = false;
  }

  _convertPcmDataToSampleBuffer(extraData->interlacedPcmDataBuffer, sampleBuffer, extraData->dataBufferNumItems);
  return result;
}

static void _freeInputSourceDataPcmFile(void* inputSourceDataPtr) {
  InputSourcePcmFileData extraData = inputSourceDataPtr;
  free(extraData->interlacedPcmDataBuffer);
  if(extraData->fileHandle != NULL) {
    fclose(extraData->fileHandle);
  }
  // TODO: malloc_error_break warns that "object was probably modified after being freed" here, not sure why
  free(extraData);
}

InputSource newInputSourcePcmFile(const CharString inputSourceName) {
  InputSource inputSource = malloc(sizeof(InputSourceMembers));

  inputSource->inputSourceType = INPUT_SOURCE_TYPE_PCM_FILE;
  inputSource->inputSourceName = newCharString();
  copyCharStrings(inputSource->inputSourceName, inputSourceName);
  // TODO: Need a way to pass in channels, bitrate, sample rate
  inputSource->numChannels = 2;
  inputSource->sampleRate = 44100.0f;

  inputSource->openInputSource = _openInputSourcePcmFile;
  inputSource->readBlock = _readBlockPcmFile;
  inputSource->freeInputSourceData = _freeInputSourceDataPcmFile;

  InputSourcePcmFileData extraData = malloc(sizeof(InputSourcePcmFileDataMembers));
  extraData->fileHandle = NULL;
  extraData->dataBufferNumItems = 0;
  extraData->interlacedPcmDataBuffer = NULL;
  inputSource->extraData = extraData;

  return inputSource;
}
