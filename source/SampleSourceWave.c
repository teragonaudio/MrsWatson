//
// SampleSourceWave.c - MrsWatson
// Created by Nik Reiman on 1/22/12.
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
#include <errno.h>
#include "AudioSettings.h"
#include "SampleSourceWave.h"
#include "EventLogger.h"
#include "SampleSource.h"
#include "CharString.h"
#include "RiffFile.h"

#if USE_LIBAUDIOFILE
#include "SampleSourceAudiofile.h"
#endif

#if ! USE_LIBAUDIOFILE
static boolByte _readWaveFileInfo(SampleSourceWaveData extraData) {
  RiffChunk chunk = newRiffChunk();
  if(readNextChunk(extraData->fileHandle, chunk, false)) {
    if(strncmp(chunk->id, "RIFF", 4)) {
      logError("WAVE file has invalid RIFF chunk descriptor");
      freeRiffChunk(chunk);
      return false;
    }
  }

  if(readNextChunk(extraData->fileHandle, chunk, true)) {
    if(strncmp(chunk->id, "fmt ", 4)) {
      logError("WAVE file has invalid format chunk header");
      freeRiffChunk(chunk);
      return false;
    }
    if(chunk->size != 16) {
      logError("WAVE file has invalid format chunk size");
      freeRiffChunk(chunk);
      return false;
    }
  }

  return true;
}
#endif

static boolByte _openSampleSourceWave(void *sampleSourcePtr, const SampleSourceOpenAs openAs) {
  SampleSource sampleSource = sampleSourcePtr;
#if USE_LIBAUDIOFILE
  SampleSourceAudiofileData extraData = sampleSource->extraData;
#else
  SampleSourceWaveData extraData = sampleSource->extraData;
#endif

  if(openAs == SAMPLE_SOURCE_OPEN_READ) {
#if USE_LIBAUDIOFILE
    extraData->fileHandle = afOpenFile(sampleSource->sourceName->data, "r", NULL);
    if(extraData->fileHandle != NULL) {
      setNumChannels(afGetVirtualChannels(extraData->fileHandle, AF_DEFAULT_TRACK));
      setSampleRate((float)afGetRate(extraData->fileHandle, AF_DEFAULT_TRACK));
    }
#else
    extraData->fileHandle = fopen(sampleSource->sourceName->data, "r");
    if(extraData->fileHandle != NULL) {
      if(_readWaveFileInfo(extraData)) {
        setNumChannels(extraData->numChannels);
        setSampleRate(extraData->sampleRate);
      }
      else {
        return false;
      }
    }
#endif
  }
  else if(openAs == SAMPLE_SOURCE_OPEN_WRITE) {
#if USE_LIBAUDIOFILE
    AFfilesetup outfileSetup = afNewFileSetup();
    afInitFileFormat(outfileSetup, AF_FILE_WAVE);
    afInitByteOrder(outfileSetup, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);
    afInitChannels(outfileSetup, AF_DEFAULT_TRACK, getNumChannels());
    afInitRate(outfileSetup, AF_DEFAULT_TRACK, getSampleRate());
    afInitSampleFormat(outfileSetup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, DEFAULT_BITRATE);
    extraData->fileHandle = afOpenFile(sampleSource->sourceName->data, "w", outfileSetup);
#else
#endif
  }
  else {
    logInternalError("Invalid type for openAs in WAVE file");
    return false;
  }

  if(extraData->fileHandle == NULL) {
    logError("WAVE file '%s' could not be opened for '%s'",
      sampleSource->sourceName->data, openAs == SAMPLE_SOURCE_OPEN_READ ? "reading" : "writing");
    return false;
  }

  sampleSource->openedAs = openAs;
  return true;
}

#if ! USE_LIBAUDIOFILE
static boolByte _readBlockFromWaveFile(void* sampleSourcePtr, SampleBuffer sampleBuffer) {
  return false;
}

static boolByte _writeBlockFromWaveFile(void* sampleSourcePtr, const SampleBuffer sampleBuffer) {
  return false;
}

static void _freeSampleSourceDataWave(void* sampleSourceDataPtr) {
  SampleSourceWaveData extraData = sampleSourceDataPtr;
  if(extraData->fileHandle != NULL) {
    fclose(extraData->fileHandle);
  }
  free(extraData->fileHandle);
}
#endif

SampleSource newSampleSourceWave(const CharString sampleSourceName) {
  SampleSource sampleSource = (SampleSource)malloc(sizeof(SampleSourceMembers));
#if USE_LIBAUDIOFILE
  SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)malloc(sizeof(SampleSourceAudiofileDataMembers));
#else
  SampleSourceWaveData extraData = (SampleSourceWaveData)malloc(sizeof(SampleSourceWaveDataMembers));
#endif

  sampleSource->sampleSourceType = SAMPLE_SOURCE_TYPE_WAVE;
  sampleSource->openedAs = SAMPLE_SOURCE_OPEN_NOT_OPENED;
  sampleSource->sourceName = newCharString();
  copyCharStrings(sampleSource->sourceName, sampleSourceName);
  sampleSource->numChannels = getNumChannels();
  sampleSource->sampleRate = getSampleRate();
  sampleSource->numFramesProcessed = 0;

  sampleSource->openSampleSource = _openSampleSourceWave;
#if USE_LIBAUDIOFILE
  sampleSource->readSampleBlock = readBlockFromAudiofile;
  sampleSource->writeSampleBlock = writeBlockFromAudiofile;
  sampleSource->freeSampleSourceData = freeSampleSourceDataAudiofile;
#else
  sampleSource->readSampleBlock = _readBlockFromWaveFile;
  sampleSource->writeSampleBlock = _writeBlockFromWaveFile;
  sampleSource->freeSampleSourceData = _freeSampleSourceDataWave;
#endif

#if USE_LIBAUDIOFILE
  extraData->fileHandle = NULL;
  extraData->interlacedBuffer = NULL;
  extraData->pcmBuffer = NULL;
#else
  extraData->fileHandle = NULL;
  extraData->audioFormat = 0;
  extraData->numChannels = 0;
  extraData->sampleRate = 0;
  extraData->byteRate = 0;
  extraData->blockAlign = 0;
  extraData->bitsPerSample = 0;
#endif

  sampleSource->extraData = extraData;

  return sampleSource;
}
