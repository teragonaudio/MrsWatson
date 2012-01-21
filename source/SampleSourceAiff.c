//
// SampleSourceAiff.c - MrsWatson
// Created by Nik Reiman on 1/21/12.
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
#include <stdint.h>
#include <string.h>
#include "AudioSettings.h"
#include "SampleSourceAiff.h"
#include "EventLogger.h"
#include "SampleSourcePcm.h"

static boolean _openSampleSourceAiff(void *sampleSourcePtr, const SampleSourceOpenAs openAs) {
  SampleSource sampleSource = sampleSourcePtr;
  SampleSourceAiffData extraData = sampleSource->extraData;

  if(openAs == SAMPLE_SOURCE_OPEN_READ) {
    extraData->fileHandle = AIFF_OpenFile(sampleSource->sourceName->data, F_RDONLY);
    if(extraData->fileHandle != NULL) {
      uint64_t numSamples;
      int numChannels, bitsPerSample, segmentSize;
      double sampleRate;
      if(AIFF_GetAudioFormat(extraData->fileHandle, &numSamples, &numChannels, &sampleRate, &bitsPerSample, &segmentSize) < 1) {
        logError("Could not read AIFF file information");
        return false;
      }
      setSampleRate((float)sampleRate);
      setNumChannels(numChannels);
    }
  }
  else if(openAs == SAMPLE_SOURCE_OPEN_WRITE) {
    extraData->fileHandle = AIFF_OpenFile(sampleSource->sourceName->data, F_WRONLY);
    if(extraData->fileHandle != NULL) {
      if(AIFF_SetAudioFormat(extraData->fileHandle, getNumChannels(), getSampleRate(), DEFAULT_BITRATE) < 1) {
        logError("Could not set AIFF file format");
        return false;
      }
      AIFF_StartWritingSamples(extraData->fileHandle);
    }
  }
  else {
    logInternalError("Invalid type for openAs in AIFF file");
    return false;
  }

  if(extraData->fileHandle == NULL) {
    logError("AIFF file '%s' could not be opened for '%s'",
      sampleSource->sourceName->data, openAs == SAMPLE_SOURCE_OPEN_READ ? "reading" : "writing");
    return false;
  }

  sampleSource->openedAs = openAs;
  return true;
}

static boolean _readBlockFromAiff(void* sampleSourcePtr, SampleBuffer sampleBuffer) {
  SampleSource sampleSource = sampleSourcePtr;
  SampleSourceAiffData extraData = sampleSource->extraData;

  int numFramesRead;
  // In mono mode, we can read directly to the sampleBuffer. Cool!
  if(getNumChannels() == 1) {
    numFramesRead = AIFF_ReadSamplesFloat(extraData->fileHandle, sampleBuffer->samples[0], getBlocksize());
  }
  // Otherwise, we must read to a temporary buffer and de-interlace.
  else {
    if(extraData->interlacedBuffer == NULL) {
      extraData->interlacedBuffer = malloc(sizeof(float) * getNumChannels() * getBlocksize());
    }
    memset(extraData->interlacedBuffer, 0, sizeof(float) * getNumChannels() * getBlocksize());

    numFramesRead = AIFF_ReadSamplesFloat(extraData->fileHandle, extraData->interlacedBuffer, getBlocksize() * getNumChannels());
    int currentInterlacedSample = 0;
    int currentDeinterlacedSample = 0;
    // Loop over the number of frames wanted, not the number we actually got. This means that the last block will
    // be partial, but then we write empty data to the end, since the interlaced buffer gets cleared above.
    while(currentInterlacedSample < getBlocksize() * getNumChannels()) {
      for(int currentChannel = 0; currentChannel < sampleBuffer->numChannels; currentChannel++) {
        sampleBuffer->samples[currentChannel][currentDeinterlacedSample] = extraData->interlacedBuffer[currentInterlacedSample++];
      }
      currentDeinterlacedSample++;
    }
  }

  sampleSource->numFramesProcessed += numFramesRead;
  if(numFramesRead == 0) {
    logDebug("End of AIFF file reached");
    return false;
  }
  else if(numFramesRead < 0) {
    logError("Error reading AIFF file");
    return false;
  }
  else {
    return true;
  }
}

static boolean _writeBlockFromAiff(void* sampleSourcePtr, const SampleBuffer sampleBuffer) {
  SampleSource sampleSource = sampleSourcePtr;
  SampleSourceAiffData extraData = sampleSource->extraData;

  if(extraData->pcmBuffer == NULL) {
    extraData->pcmBuffer = malloc(sizeof(short) * getNumChannels() * getBlocksize());
  }
  memset(extraData->pcmBuffer, 0, sizeof(short) * getNumChannels() * getBlocksize());
  convertSampleBufferToPcmData(sampleBuffer, extraData->pcmBuffer);

  int result = AIFF_WriteSamples(extraData->fileHandle, extraData->pcmBuffer, sizeof(short) * getNumChannels() * getBlocksize());
  sampleSource->numFramesProcessed += getBlocksize() * getNumChannels();
  return (result == 1);
}

static void _freeInputSourceDataAiff(void* sampleSourceDataPtr) {
  SampleSourceAiffData extraData = sampleSourceDataPtr;
  if(extraData->fileHandle != NULL) {
    // Semi-cheap hack to see if file was opened for writing
    if(extraData->pcmBuffer != NULL) {
      AIFF_EndWritingSamples(extraData->fileHandle);
    }
    AIFF_CloseFile(extraData->fileHandle);
  }
  if(extraData->interlacedBuffer != NULL) {
    free(extraData->interlacedBuffer);
  }
  if(extraData->pcmBuffer != NULL) {
    free(extraData->pcmBuffer);
  }
  free(extraData);
}

SampleSource newSampleSourceAiff(const CharString sampleSourceName) {
  SampleSource sampleSource = malloc(sizeof(SampleSourceMembers));

  sampleSource->sampleSourceType = SAMPLE_SOURCE_TYPE_AIFF;
  sampleSource->openedAs = SAMPLE_SOURCE_OPEN_NOT_OPENED;
  sampleSource->sourceName = newCharString();
  copyCharStrings(sampleSource->sourceName, sampleSourceName);
  sampleSource->numChannels = getNumChannels();
  sampleSource->sampleRate = getSampleRate();
  sampleSource->numFramesProcessed = 0;

  sampleSource->openSampleSource = _openSampleSourceAiff;
  sampleSource->readSampleBlock = _readBlockFromAiff;
  sampleSource->writeSampleBlock = _writeBlockFromAiff;
  sampleSource->freeSampleSourceData = _freeInputSourceDataAiff;

  SampleSourceAiffData extraData = malloc(sizeof(SampleSourceAiffDataMembers));
  extraData->fileHandle = NULL;
  extraData->interlacedBuffer = NULL;
  extraData->pcmBuffer = NULL;
  sampleSource->extraData = extraData;

  return sampleSource;
}
