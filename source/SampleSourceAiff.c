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
#include <stdlib.h>
#include "AudioSettings.h"
#include "SampleSourceAiff.h"
#include "EventLogger.h"
#include "SampleSource.h"
#include "SampleSourcePcm.h"
#include "PlatformUtilities.h"

#if HAVE_LIBAUDIOFILE
#include "SampleSourceAudiofile.h"

#if LINUX
#include <audiofile.h>
#else
#include "audiofile.h"
#endif
#endif

static boolByte _openSampleSourceAiff(void *sampleSourcePtr, const SampleSourceOpenAs openAs) {
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
#if HAVE_LIBAUDIOFILE
  SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)(sampleSource->extraData);
#else
  SampleSourcePcmData extraData = (SampleSourcePcmData)(sampleSource->extraData);
#endif

  if(openAs == SAMPLE_SOURCE_OPEN_READ) {
#if HAVE_LIBAUDIOFILE
    extraData->fileHandle = afOpenFile(sampleSource->sourceName->data, "r", NULL);
    if(extraData->fileHandle != NULL) {
      setNumChannels(afGetVirtualChannels(extraData->fileHandle, AF_DEFAULT_TRACK));
      setSampleRate((float)afGetRate(extraData->fileHandle, AF_DEFAULT_TRACK));
    }
#else
    logInternalError("Executable was not built with a library to read AIFF files");
#endif
  }
  else if(openAs == SAMPLE_SOURCE_OPEN_WRITE) {
#if HAVE_LIBAUDIOFILE
    AFfilesetup outfileSetup = afNewFileSetup();
    afInitFileFormat(outfileSetup, AF_FILE_AIFF);
    afInitByteOrder(outfileSetup, AF_DEFAULT_TRACK, AF_BYTEORDER_BIGENDIAN);
    afInitChannels(outfileSetup, AF_DEFAULT_TRACK, getNumChannels());
    afInitRate(outfileSetup, AF_DEFAULT_TRACK, getSampleRate());
    afInitSampleFormat(outfileSetup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, DEFAULT_BITRATE);
    extraData->fileHandle = afOpenFile(sampleSource->sourceName->data, "w", outfileSetup);
#else
    logInternalError("Executable was not built with a library to write AIFF files");
#endif
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

static boolByte _readBlockFromAiffFile(void* sampleSourcePtr, SampleBuffer sampleBuffer) {
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourcePcmData extraData = (SampleSourcePcmData)(sampleSource->extraData);
  return readPcmDataFromFile(extraData, sampleBuffer, &(sampleSource->numFramesProcessed));
}

static boolByte _writeBlockToAiffFile(void* sampleSourcePtr, const SampleBuffer sampleBuffer) {
  boolByte result;
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourcePcmData extraData = (SampleSourcePcmData)(sampleSource->extraData);
  result = writePcmDataToFile(extraData, sampleBuffer, &(extraData->numSamplesWritten));
  sampleSource->numFramesProcessed = extraData->numSamplesWritten;
  return result;
}

static void _freeSampleSourceDataAiff(void* sampleSourceDataPtr) {
  freeSampleSourceDataPcm(sampleSourceDataPtr);
}

SampleSource newSampleSourceAiff(const CharString sampleSourceName) {
  SampleSource sampleSource = (SampleSource)malloc(sizeof(SampleSourceMembers));
#if HAVE_LIBAUDIOFILE
  SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)malloc(sizeof(SampleSourceAudiofileDataMembers));
#else
  SampleSourcePcmData extraData = (SampleSourcePcmData)malloc(sizeof(SampleSourcePcmDataMembers));
#endif

  sampleSource->sampleSourceType = SAMPLE_SOURCE_TYPE_AIFF;
  sampleSource->openedAs = SAMPLE_SOURCE_OPEN_NOT_OPENED;
  sampleSource->sourceName = newCharString();
  copyCharStrings(sampleSource->sourceName, sampleSourceName);
  sampleSource->numChannels = getNumChannels();
  sampleSource->sampleRate = getSampleRate();
  sampleSource->numFramesProcessed = 0;

  sampleSource->openSampleSource = _openSampleSourceAiff;
#if HAVE_LIBAUDIOFILE
  sampleSource->readSampleBlock = readBlockFromAudiofile;
  sampleSource->writeSampleBlock = writeBlockToAudiofile;
  sampleSource->freeSampleSourceData = freeSampleSourceDataAudiofile;
#else
  sampleSource->readSampleBlock = _readBlockFromAiffFile;
  sampleSource->writeSampleBlock = _writeBlockToAiffFile;
  sampleSource->freeSampleSourceData = _freeSampleSourceDataAiff;
#endif

#if HAVE_LIBAUDIOFILE
  extraData->fileHandle = NULL;
  extraData->interlacedBuffer = NULL;
  extraData->pcmBuffer = NULL;
#else
  extraData->isStream = false;
  extraData->isLittleEndian = false;
  extraData->fileHandle = NULL;
  extraData->dataBufferNumItems = 0;
  extraData->interlacedPcmDataBuffer = NULL;

  extraData->numChannels = (unsigned short)getNumChannels();
  extraData->sampleRate = (unsigned int)getSampleRate();
  extraData->bitsPerSample = 16;
  extraData->numSamplesWritten = 0;
#endif

  sampleSource->extraData = extraData;

  return sampleSource;
}
