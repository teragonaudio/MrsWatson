//
// SampleSourcePcmFile.c - MrsWatson
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
#include "SampleSourcePcmFile.h"
#include "EventLogger.h"
#include "AudioSettings.h"

static boolean _openSampleSourcePcmFile(void* sampleSourcePtr, const SampleSourceOpenAs openAs) {
  SampleSource sampleSource = sampleSourcePtr;
  SampleSourcePcmFileData extraData = sampleSource->extraData;

  extraData->dataBufferNumItems = 0;
  if(openAs == SAMPLE_SOURCE_OPEN_READ) {
    extraData->fileHandle = fopen(sampleSource->sourceName->data, "rb");
  }
  else if(openAs == SAMPLE_SOURCE_OPEN_WRITE) {
    extraData->fileHandle = fopen(sampleSource->sourceName->data, "wb");
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
  while(currentInterlacedSample < numInterlacedSamples) {
    for(int currentChannel = 0; currentChannel < sampleBuffer->numChannels; currentChannel++) {
      Sample convertedSample = (Sample)inPcmSamples[currentInterlacedSample++] / 32767.0f;
      // Apply brickwall limiter to prevent clipping
      if(convertedSample > 1.0f) {
        convertedSample = 1.0f;
      }
      else if(convertedSample < -1.0f) {
        convertedSample = -1.0f;
      }
      sampleBuffer->samples[currentChannel][currentDeinterlacedSample] = convertedSample;
    }
    currentDeinterlacedSample++;
  }
}

static boolean _readBlockFromPcmFile(void* sampleSourcePtr, SampleBuffer sampleBuffer) {
  SampleSource sampleSource = sampleSourcePtr;
  SampleSourcePcmFileData extraData = sampleSource->extraData;
  if(extraData->dataBufferNumItems == 0) {
    extraData->dataBufferNumItems = (size_t)(sampleBuffer->numChannels * sampleBuffer->blocksize);
    extraData->interlacedPcmDataBuffer = malloc(sizeof(short) * extraData->dataBufferNumItems);
  }

  // Clear the PCM data buffer, or else the last block will have dirty samples in the end
  memset(extraData->interlacedPcmDataBuffer, 0, sizeof(short) * extraData->dataBufferNumItems);

  boolean result = true;
  size_t pcmSamplesRead = fread(extraData->interlacedPcmDataBuffer, sizeof(short), extraData->dataBufferNumItems, extraData->fileHandle);
  if(pcmSamplesRead < extraData->dataBufferNumItems) {
    logDebug("End of PCM file reached");
    result = false;
  }
  sampleSource->numFramesProcessed += pcmSamplesRead;
  logDebug("Read %d samples from PCM file", pcmSamplesRead);

  _convertPcmDataToSampleBuffer(extraData->interlacedPcmDataBuffer, sampleBuffer);
  return result;
}

static void _convertSampleBufferToPcmData(const SampleBuffer sampleBuffer, short* outPcmSamples) {
  int currentInterlacedSample = 0;
  for(int currentSample = 0; currentSample < sampleBuffer->blocksize; currentSample++) {
    for(int currentChannel = 0; currentChannel < sampleBuffer->numChannels; currentChannel++) {
      Sample sample = sampleBuffer->samples[currentChannel][currentSample];
      if(sample > 1.0f) {
        sample = 1.0f;
      }
      else if(sample < -1.0f) {
        sample = -1.0f;
      }
      outPcmSamples[currentInterlacedSample++] = (short)(sample * 32767.0f);
    }
  }
}

static boolean _writeBlockFromPcmFile(void* sampleSourcePtr, const SampleBuffer sampleBuffer) {
  SampleSource sampleSource = sampleSourcePtr;
  SampleSourcePcmFileData extraData = sampleSource->extraData;
  if(extraData->dataBufferNumItems == 0) {
    extraData->dataBufferNumItems = (size_t)(sampleBuffer->numChannels * sampleBuffer->blocksize);
    extraData->interlacedPcmDataBuffer = malloc(sizeof(short) * extraData->dataBufferNumItems);
  }

  // Clear the PCM data buffer just to be safe
  memset(extraData->interlacedPcmDataBuffer, 0, sizeof(short) * extraData->dataBufferNumItems);

  _convertSampleBufferToPcmData(sampleBuffer, extraData->interlacedPcmDataBuffer);
  size_t pcmSamplesWritten = fwrite(extraData->interlacedPcmDataBuffer, sizeof(short), extraData->dataBufferNumItems, extraData->fileHandle);
  if(pcmSamplesWritten < extraData->dataBufferNumItems) {
    logWarn("Short write to PCM file");
    return false;
  }

  sampleSource->numFramesProcessed += pcmSamplesWritten;
  logDebug("Wrote %d samples to PCM file", pcmSamplesWritten);
  return true;
}

static void _freeInputSourceDataPcmFile(void* sampleSourceDataPtr) {
  SampleSourcePcmFileData extraData = sampleSourceDataPtr;
  free(extraData->interlacedPcmDataBuffer);
  if(extraData->fileHandle != NULL) {
    fclose(extraData->fileHandle);
  }
  free(extraData);
}

SampleSource newSampleSourcePcmFile(const CharString sampleSourceName) {
  SampleSource sampleSource = malloc(sizeof(SampleSourceMembers));

  sampleSource->sampleSourceType = SAMPLE_SOURCE_TYPE_PCM_FILE;
  sampleSource->openedAs = SAMPLE_SOURCE_OPEN_NOT_OPENED;
  sampleSource->sourceName = newCharString();
  copyCharStrings(sampleSource->sourceName, sampleSourceName);
  sampleSource->numChannels = getNumChannels();
  sampleSource->sampleRate = getSampleRate();
  sampleSource->numFramesProcessed = 0;

  sampleSource->openSampleSource = _openSampleSourcePcmFile;
  sampleSource->readSampleBlock = _readBlockFromPcmFile;
  sampleSource->writeSampleBlock = _writeBlockFromPcmFile;
  sampleSource->freeSampleSourceData = _freeInputSourceDataPcmFile;

  SampleSourcePcmFileData extraData = malloc(sizeof(SampleSourcePcmFileDataMembers));
  extraData->fileHandle = NULL;
  extraData->dataBufferNumItems = 0;
  extraData->interlacedPcmDataBuffer = NULL;
  sampleSource->extraData = extraData;

  return sampleSource;
}
