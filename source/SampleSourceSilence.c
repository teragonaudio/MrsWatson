//
// SampleSourceSilence.c - MrsWatson
// Created by Nik Reiman on 1/5/12.
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
#include "SampleSourceSilence.h"
#include "AudioSettings.h"

static boolByte _openSampleSourceSilence(void* sampleSourcePtr, const SampleSourceOpenAs openAs) {
  SampleSource sampleSource = sampleSourcePtr;
  sampleSource->openedAs = openAs;
  return true;
}

void _closeSampleSourceSilence(void* sampleSourcePtr) {
  return true;
}

static boolByte _readBlockFromSilence(void* sampleSourcePtr, SampleBuffer sampleBuffer) {
  clearSampleBuffer(sampleBuffer);
  return true;
}

static boolByte _writeBlockToSilence(void* sampleSourcePtr, const SampleBuffer sampleBuffer) {
  return true;
}

static void _freeInputSourceDataSilence(void* sampleSourceDataPtr) {
}

SampleSource newSampleSourceSilence(void) {
  SampleSource sampleSource = malloc(sizeof(SampleSourceMembers));

  sampleSource->sampleSourceType = SAMPLE_SOURCE_TYPE_SILENCE;
  sampleSource->openedAs = SAMPLE_SOURCE_OPEN_NOT_OPENED;
  sampleSource->sourceName = newCharString();
  copyToCharString(sampleSource->sourceName, "(silence)");
  sampleSource->numChannels = getNumChannels();
  sampleSource->sampleRate = getSampleRate();
  sampleSource->numSamplesProcessed = 0;

  sampleSource->openSampleSource = _openSampleSourceSilence;
  sampleSource->closeSampleSource = _closeSampleSourceSilence;
  sampleSource->readSampleBlock = _readBlockFromSilence;
  sampleSource->writeSampleBlock = _writeBlockToSilence;
  sampleSource->freeSampleSourceData = _freeInputSourceDataSilence;

  return sampleSource;
}
