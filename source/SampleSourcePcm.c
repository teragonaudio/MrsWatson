//
// SampleSourcePcm.c - MrsWatson
// Created by Nik Reiman on 1/2/12.
// Copyright (c) 2012 Teragon Audio. All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SampleSourcePcm.h"
#include "EventLogger.h"
#include "AudioSettings.h"

static boolean _openSampleSourcePcm(void* sampleSourcePtr, const SampleSourceOpenAs openAs) {
  SampleSource sampleSource = sampleSourcePtr;
  SampleSourcePcmData extraData = sampleSource->extraData;

  extraData->dataBufferNumItems = 0;
  if(openAs == SAMPLE_SOURCE_OPEN_READ) {
    if(isCharStringEqualToCString(sampleSource->sourceName, "-", false)) {
      extraData->fileHandle = stdin;
      strncpy(sampleSource->sourceName->data, "stdin", (size_t)sampleSource->sourceName->capacity);
      extraData->isStream = true;
    }
    else {
      extraData->fileHandle = fopen(sampleSource->sourceName->data, "rb");
    }
  }
  else if(openAs == SAMPLE_SOURCE_OPEN_WRITE) {
    if(isCharStringEqualToCString(sampleSource->sourceName, "-", false)) {
      extraData->fileHandle = stdout;
      strncpy(sampleSource->sourceName->data, "stdout", (size_t)sampleSource->sourceName->capacity);
      extraData->isStream = true;
    }
    else {
      extraData->fileHandle = fopen(sampleSource->sourceName->data, "wb");
    }
  }
  else {
    logInternalError("Invalid type for openAs in PCM file");
    return false;
  }

  if(extraData->fileHandle == NULL) {
    logError("PCM File '%s' could not be opened for %s",
      sampleSource->sourceName->data, openAs == SAMPLE_SOURCE_OPEN_READ ? "reading" : "writing");
    return false;
  }

  sampleSource->openedAs = openAs;
  return true;
}

static void _convertPcmDataToSampleBuffer(const short* inPcmSamples, SampleBuffer sampleBuffer) {
  int numInterlacedSamples = sampleBuffer->numChannels * sampleBuffer->blocksize;
  int currentInterlacedSample = 0;
  int currentDeinterlacedSample = 0;
  int currentChannel;
  while(currentInterlacedSample < numInterlacedSamples) {
    for(currentChannel = 0; currentChannel < sampleBuffer->numChannels; currentChannel++) {
      Sample convertedSample = (Sample)inPcmSamples[currentInterlacedSample++] / 32767.0f;
#if USE_BRICKWALL_LIMITER
      if(convertedSample > 1.0f) {
        convertedSample = 1.0f;
      }
      else if(convertedSample < -1.0f) {
        convertedSample = -1.0f;
      }
#endif
      sampleBuffer->samples[currentChannel][currentDeinterlacedSample] = convertedSample;
    }
    currentDeinterlacedSample++;
  }
}

static boolean _readBlockFromPcm(void* sampleSourcePtr, SampleBuffer sampleBuffer) {
  boolean result = true;
  size_t pcmFramesRead = 0;
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourcePcmData extraData = (SampleSourcePcmData)(sampleSource->extraData);

  if(extraData->dataBufferNumItems == 0) {
    extraData->dataBufferNumItems = (size_t)(sampleBuffer->numChannels * sampleBuffer->blocksize);
    extraData->interlacedPcmDataBuffer = (short*)malloc(sizeof(short) * extraData->dataBufferNumItems);
  }

  // Clear the PCM data buffer, or else the last block will have dirty samples in the end
  memset(extraData->interlacedPcmDataBuffer, 0, sizeof(short) * extraData->dataBufferNumItems);

  pcmFramesRead = fread(extraData->interlacedPcmDataBuffer, sizeof(short), extraData->dataBufferNumItems, extraData->fileHandle);
  if(pcmFramesRead < extraData->dataBufferNumItems) {
    logDebug("End of PCM file reached");
    result = false;
  }
  sampleSource->numFramesProcessed += pcmFramesRead;
  logDebug("Read %d sample frames from PCM file", pcmFramesRead);

  _convertPcmDataToSampleBuffer(extraData->interlacedPcmDataBuffer, sampleBuffer);
  return result;
}

void convertSampleBufferToPcmData(const SampleBuffer sampleBuffer, short* outPcmSamples) {
  int currentInterlacedSample = 0;
  int currentSample = 0;
  int currentChannel = 0;
  Sample sample;
  for(currentSample = 0; currentSample < sampleBuffer->blocksize; currentSample++) {
    for(currentChannel = 0; currentChannel < sampleBuffer->numChannels; currentChannel++) {
      sample = sampleBuffer->samples[currentChannel][currentSample];
#if USE_BRICKWALL_LIMITER
      if(sample > 1.0f) {
        sample = 1.0f;
      }
      else if(sample < -1.0f) {
        sample = -1.0f;
      }
#endif
      outPcmSamples[currentInterlacedSample++] = (short)(sample * 32767.0f);
    }
  }
}

static boolean _writeBlockFromPcm(void* sampleSourcePtr, const SampleBuffer sampleBuffer) {
  size_t pcmFramesWritten = 0;
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourcePcmData extraData = (SampleSourcePcmData)(sampleSource->extraData);
  if(extraData->dataBufferNumItems == 0) {
    extraData->dataBufferNumItems = (size_t)(sampleBuffer->numChannels * sampleBuffer->blocksize);
    extraData->interlacedPcmDataBuffer = malloc(sizeof(short) * extraData->dataBufferNumItems);
  }

  // Clear the PCM data buffer just to be safe
  memset(extraData->interlacedPcmDataBuffer, 0, sizeof(short) * extraData->dataBufferNumItems);

  convertSampleBufferToPcmData(sampleBuffer, extraData->interlacedPcmDataBuffer);
  pcmFramesWritten = fwrite(extraData->interlacedPcmDataBuffer, sizeof(short), extraData->dataBufferNumItems, extraData->fileHandle);
  if(pcmFramesWritten < extraData->dataBufferNumItems) {
    logWarn("Short write to PCM file");
    return false;
  }

  sampleSource->numFramesProcessed += pcmFramesWritten;
  logDebug("Wrote %d sample frames to PCM file", pcmFramesWritten);
  return true;
}

static void _freeSampleSourceDataPcm(void* sampleSourceDataPtr) {
  SampleSourcePcmData extraData = (SampleSourcePcmData)sampleSourceDataPtr;
  free(extraData->interlacedPcmDataBuffer);
  if(extraData->fileHandle != NULL) {
    fclose(extraData->fileHandle);
  }
  free(extraData);
}

SampleSource newSampleSourcePcm(const CharString sampleSourceName) {
  SampleSource sampleSource = (SampleSource)malloc(sizeof(SampleSourceMembers));
  SampleSourcePcmData extraData = (SampleSourcePcmData)malloc(sizeof(SampleSourcePcmDataMembers));

  sampleSource->sampleSourceType = SAMPLE_SOURCE_TYPE_PCM;
  sampleSource->openedAs = SAMPLE_SOURCE_OPEN_NOT_OPENED;
  sampleSource->sourceName = newCharString();
  copyCharStrings(sampleSource->sourceName, sampleSourceName);
  sampleSource->numChannels = getNumChannels();
  sampleSource->sampleRate = getSampleRate();
  sampleSource->numFramesProcessed = 0;

  sampleSource->openSampleSource = _openSampleSourcePcm;
  sampleSource->readSampleBlock = _readBlockFromPcm;
  sampleSource->writeSampleBlock = _writeBlockFromPcm;
  sampleSource->freeSampleSourceData = _freeSampleSourceDataPcm;

  extraData->isStream = false;
  extraData->fileHandle = NULL;
  extraData->dataBufferNumItems = 0;
  extraData->interlacedPcmDataBuffer = NULL;
  sampleSource->extraData = extraData;

  return sampleSource;
}
