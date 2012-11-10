//
// SampleSourceFlac.c - MrsWatson
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
#include "SampleSourceFlac.h"
#include "EventLogger.h"

#if HAVE_LIBFLAC

static boolByte _openSampleSourceFlac(void* sampleSourcePtr, const SampleSourceOpenAs openAs) {
  logUnsupportedFeature("Flac file I/O");
  return false;
}

static boolByte _readBlockFromFlacFile(void* sampleSourcePtr, SampleBuffer sampleBuffer) {
  logUnsupportedFeature("Flac file I/O");
  return false;
}

static boolByte _writeBlockToFlacFile(void* sampleSourcePtr, const SampleBuffer sampleBuffer) {
  logUnsupportedFeature("Flac file I/O");
  return false;
}

static void _freeSampleSourceFlac(void* sampleSourceDataPtr) {
  
}

SampleSource newSampleSourceFlac(const CharString sampleSourceName) {
  SampleSource sampleSource = (SampleSource)malloc(sizeof(SampleSourceMembers));
  
  sampleSource->sampleSourceType = SAMPLE_SOURCE_TYPE_FLAC;
  
  sampleSource->openSampleSource = _openSampleSourceFlac;
  sampleSource->readSampleBlock = _readBlockFromFlacFile;
  sampleSource->writeSampleBlock = _writeBlockToFlacFile;
  sampleSource->freeSampleSourceData = _freeSampleSourceFlac;
  
  return sampleSource;
}

#endif
