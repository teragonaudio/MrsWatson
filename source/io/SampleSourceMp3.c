//
// SampleSourceMp3.c - MrsWatson
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

#include <stdlib.h>

#include "io/SampleSourceMp3.h"
#include "logging/EventLogger.h"

#if HAVE_LIBLAME

static boolByte _openSampleSourceMp3(void* sampleSourcePtr, const SampleSourceOpenAs openAs) {
  logUnsupportedFeature("MP3 file I/O");
  return false;
}

static boolByte _readBlockFromMp3File(void* sampleSourcePtr, SampleBuffer sampleBuffer) {
  logUnsupportedFeature("MP3 file I/O");
  return false;
}

static boolByte _writeBlockToMp3File(void* sampleSourcePtr, const SampleBuffer sampleBuffer) {
  logUnsupportedFeature("MP3 file I/O");
  return false;
}

static void _freeSampleSourceMp3(void* sampleSourceDataPtr) {
  
}

SampleSource newSampleSourceMp3(const CharString sampleSourceName) {
  SampleSource sampleSource = (SampleSource)malloc(sizeof(SampleSourceMembers));

  sampleSource->sampleSourceType = SAMPLE_SOURCE_TYPE_MP3;

  sampleSource->openSampleSource = _openSampleSourceMp3;
  sampleSource->readSampleBlock = _readBlockFromMp3File;
  sampleSource->writeSampleBlock = _writeBlockToMp3File;
  sampleSource->freeSampleSourceData = _freeSampleSourceMp3;

  return sampleSource;
}

#endif
