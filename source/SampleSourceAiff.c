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
#include <stdlib.h>
#include "AudioSettings.h"
#include "SampleSourceAiff.h"
#include "SampleSourceAudiofile.h"
#include "EventLogger.h"

static boolean _openSampleSourceAiff(void *sampleSourcePtr, const SampleSourceOpenAs openAs) {
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)(sampleSource->extraData);

  if(openAs == SAMPLE_SOURCE_OPEN_READ) {
    extraData->fileHandle = afOpenFile(sampleSource->sourceName->data, "r", NULL);
    if(extraData->fileHandle != NULL) {
      setNumChannels(afGetVirtualChannels(extraData->fileHandle, AF_DEFAULT_TRACK));
      setSampleRate((float)afGetRate(extraData->fileHandle, AF_DEFAULT_TRACK));
    }
  }
  else if(openAs == SAMPLE_SOURCE_OPEN_WRITE) {
    AFfilesetup outfileSetup = afNewFileSetup();
    afInitFileFormat(outfileSetup, AF_FILE_AIFF);
    afInitByteOrder(outfileSetup, AF_DEFAULT_TRACK, AF_BYTEORDER_BIGENDIAN);
    afInitChannels(outfileSetup, AF_DEFAULT_TRACK, getNumChannels());
    afInitRate(outfileSetup, AF_DEFAULT_TRACK, getSampleRate());
    afInitSampleFormat(outfileSetup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, DEFAULT_BITRATE);
    extraData->fileHandle = afOpenFile(sampleSource->sourceName->data, "w", outfileSetup);
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

SampleSource newSampleSourceAiff(const CharString sampleSourceName) {
  SampleSource sampleSource = (SampleSource)malloc(sizeof(SampleSourceMembers));
  SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)malloc(sizeof(SampleSourceAudiofileDataMembers));

  sampleSource->sampleSourceType = SAMPLE_SOURCE_TYPE_AIFF;
  sampleSource->openedAs = SAMPLE_SOURCE_OPEN_NOT_OPENED;
  sampleSource->sourceName = newCharString();
  copyCharStrings(sampleSource->sourceName, sampleSourceName);
  sampleSource->numChannels = getNumChannels();
  sampleSource->sampleRate = getSampleRate();
  sampleSource->numFramesProcessed = 0;

  sampleSource->openSampleSource = _openSampleSourceAiff;
  sampleSource->readSampleBlock = readBlockFromAudiofile;
  sampleSource->writeSampleBlock = writeBlockFromAudiofile;
  sampleSource->freeSampleSourceData = freeSampleSourceDataAudiofile;

  extraData->fileHandle = NULL;
  extraData->interlacedBuffer = NULL;
  extraData->pcmBuffer = NULL;
  sampleSource->extraData = extraData;

  return sampleSource;
}
