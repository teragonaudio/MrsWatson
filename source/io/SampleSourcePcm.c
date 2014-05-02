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

#include "audio/AudioSettings.h"
#include "base/PlatformUtilities.h"
#include "io/SampleSourcePcm.h"
#include "logging/EventLogger.h"

static boolByte openSampleSourcePcm(void* sampleSourcePtr, const SampleSourceOpenAs openAs) {
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourcePcmData extraData = (SampleSourcePcmData)(sampleSource->extraData);

  extraData->dataBufferNumItems = 0;
  if(openAs == SAMPLE_SOURCE_OPEN_READ) {
    if(charStringIsEqualToCString(sampleSource->sourceName, "-", false)) {
      extraData->fileHandle = stdin;
      charStringCopyCString(sampleSource->sourceName, "stdin");
      extraData->isStream = true;
    }
    else {
      extraData->fileHandle = fopen(sampleSource->sourceName->data, "rb");
    }
  }
  else if(openAs == SAMPLE_SOURCE_OPEN_WRITE) {
    if(charStringIsEqualToCString(sampleSource->sourceName, "-", false)) {
      extraData->fileHandle = stdout;
      charStringCopyCString(sampleSource->sourceName, "stdout");
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

size_t sampleSourcePcmRead(SampleSourcePcmData self, SampleBuffer sampleBuffer) {
  size_t pcmSamplesRead = 0;

  if(self == NULL || self->fileHandle == NULL) {
    logCritical("Corrupt PCM data structure");
    return 0;
  }

  if(self->dataBufferNumItems < (size_t)(sampleBuffer->numChannels * sampleBuffer->blocksize)) {
    self->dataBufferNumItems = (size_t)(sampleBuffer->numChannels * sampleBuffer->blocksize);
    self->interlacedPcmDataBuffer = (short*)realloc(self->interlacedPcmDataBuffer, sizeof(short) * self->dataBufferNumItems);
  }

  // Clear the PCM data buffer, or else the last block will have dirty samples in the end
  memset(self->interlacedPcmDataBuffer, 0, sizeof(short) * self->dataBufferNumItems);

  pcmSamplesRead = fread(self->interlacedPcmDataBuffer, sizeof(short), self->dataBufferNumItems, self->fileHandle);
  if(pcmSamplesRead < self->dataBufferNumItems) {
    logDebug("End of PCM file reached");
    // Set the blocksize of the sample buffer to be the number of frames read
    sampleBuffer->blocksize = pcmSamplesRead / sampleBuffer->numChannels;
  }
  logDebug("Read %d samples from PCM file", pcmSamplesRead);

  sampleBufferCopyPcmSamples(sampleBuffer, self->interlacedPcmDataBuffer);
  return pcmSamplesRead;
}

static boolByte readBlockFromPcmFile(void* sampleSourcePtr, SampleBuffer sampleBuffer) {
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourcePcmData extraData = (SampleSourcePcmData)(sampleSource->extraData);
  unsigned long originalBlocksize = sampleBuffer->blocksize;
  size_t samplesRead = sampleSourcePcmRead(extraData, sampleBuffer);
  sampleSource->numSamplesProcessed += samplesRead;
  return (originalBlocksize == sampleBuffer->blocksize);
}

size_t sampleSourcePcmWrite(SampleSourcePcmData self, const SampleBuffer sampleBuffer) {
  size_t pcmSamplesWritten = 0;
  size_t numSamplesToWrite = (size_t)(sampleBuffer->numChannels * sampleBuffer->blocksize);

  if(self == NULL || self->fileHandle == NULL) {
    logCritical("Corrupt PCM data structure");
    return false;
  }

  if(self->dataBufferNumItems < (size_t)(sampleBuffer->numChannels * sampleBuffer->blocksize)) {
    self->dataBufferNumItems = (size_t)(sampleBuffer->numChannels * sampleBuffer->blocksize);
    self->interlacedPcmDataBuffer = (short*)realloc(self->interlacedPcmDataBuffer, sizeof(short) * self->dataBufferNumItems);
  }

  // Clear the PCM data buffer just to be safe
  memset(self->interlacedPcmDataBuffer, 0, sizeof(short) * self->dataBufferNumItems);

  sampleBufferGetPcmSamples(sampleBuffer, self->interlacedPcmDataBuffer, self->isLittleEndian != isHostLittleEndian());
  pcmSamplesWritten = fwrite(self->interlacedPcmDataBuffer, sizeof(short), numSamplesToWrite, self->fileHandle);
  if(pcmSamplesWritten < numSamplesToWrite) {
    logWarn("Short write to PCM file");
    return pcmSamplesWritten;
  }

  logDebug("Wrote %d samples to PCM file", pcmSamplesWritten);
  return pcmSamplesWritten;
}

static boolByte writeBlockToPcmFile(void* sampleSourcePtr, const SampleBuffer sampleBuffer) {
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourcePcmData extraData = (SampleSourcePcmData)(sampleSource->extraData);
  unsigned int samplesWritten = (int)sampleSourcePcmWrite(extraData, sampleBuffer);
  sampleSource->numSamplesProcessed += samplesWritten;
  return (samplesWritten == sampleBuffer->blocksize);
}

static void _closeSampleSourcePcm(void* sampleSourcePtr) {
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourcePcmData extraData = (SampleSourcePcmData)sampleSource->extraData;
  if(extraData->fileHandle != NULL) {
    fclose(extraData->fileHandle);
  }
}

void sampleSourcePcmSetSampleRate(void* sampleSourcePtr, double sampleRate) {
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourcePcmData extraData = (SampleSourcePcmData)sampleSource->extraData;
  extraData->sampleRate = (unsigned int)sampleRate;
}

void sampleSourcePcmSetNumChannels(void* sampleSourcePtr, int numChannels) {
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourcePcmData extraData = (SampleSourcePcmData)sampleSource->extraData;
  extraData->numChannels = numChannels;
}

void freeSampleSourceDataPcm(void* sampleSourceDataPtr) {
  SampleSourcePcmData extraData = (SampleSourcePcmData)sampleSourceDataPtr;
  if(extraData->interlacedPcmDataBuffer != NULL) {
    free(extraData->interlacedPcmDataBuffer);
    extraData->interlacedPcmDataBuffer = NULL;
  }
  free(extraData);
}

SampleSource _newSampleSourcePcm(const CharString sampleSourceName) {
  SampleSource sampleSource = (SampleSource)malloc(sizeof(SampleSourceMembers));
  SampleSourcePcmData extraData = (SampleSourcePcmData)malloc(sizeof(SampleSourcePcmDataMembers));

  sampleSource->sampleSourceType = SAMPLE_SOURCE_TYPE_PCM;
  sampleSource->openedAs = SAMPLE_SOURCE_OPEN_NOT_OPENED;
  sampleSource->sourceName = newCharString();
  charStringCopy(sampleSource->sourceName, sampleSourceName);
  sampleSource->numSamplesProcessed = 0;

  sampleSource->openSampleSource = openSampleSourcePcm;
  sampleSource->readSampleBlock = readBlockFromPcmFile;
  sampleSource->writeSampleBlock = writeBlockToPcmFile;
  sampleSource->closeSampleSource = _closeSampleSourcePcm;
  sampleSource->freeSampleSourceData = freeSampleSourceDataPcm;

  extraData->isStream = false;
  extraData->isLittleEndian = true;
  extraData->fileHandle = NULL;
  extraData->dataBufferNumItems = 0;
  extraData->interlacedPcmDataBuffer = NULL;

  extraData->numChannels = (unsigned short)getNumChannels();
  extraData->sampleRate = (unsigned int)getSampleRate();
  extraData->bitsPerSample = 16;
  sampleSource->extraData = extraData;

  return sampleSource;
}
