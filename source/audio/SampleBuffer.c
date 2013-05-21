//
// SampleBuffer.c - MrsWatson
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

#include "audio/SampleBuffer.h"
#include "logging/EventLogger.h"

SampleBuffer newSampleBuffer(int numChannels, int blocksize) {
  SampleBuffer sampleBuffer = NULL;
  int i;

  if(numChannels <= 0) {
    logError("Cannot create sample buffer with channel count %d", numChannels);
    return NULL;
  }
  else if(numChannels > 2) {
    logUnsupportedFeature("Channel count >2");
    return NULL;
  }
  else if(blocksize <= 0) {
    logError("Cannot create sample buffer with blocksize %d", blocksize);
    return NULL;
  }

  sampleBuffer = (SampleBuffer)malloc(sizeof(SampleBufferMembers));
  sampleBuffer->numChannels = numChannels;
  sampleBuffer->blocksize = blocksize;

  // Bah, VST plugins are essentially hardcoded to be stereo, so we need to force
  // two channels here even if we have a mono input source. This is not so flexible
  // but should at least allow mono input sources to work correctly.
  // Also note that internally, the numChannels member should still be correct,
  // this is just a workaround to prevent segfaults.
  sampleBuffer->samples = (Samples*)malloc(sizeof(Samples) * 2);
  for(i = 0; i < 2; i++) {
    sampleBuffer->samples[i] = (Samples)malloc(sizeof(Sample) * blocksize);
  }
  sampleBufferClear(sampleBuffer);

  return sampleBuffer;
}

void sampleBufferClear(SampleBuffer self) {
  int i;
  for(i = 0; i < self->numChannels; i++) {
    memset(self->samples[i], 0, sizeof(Sample) * self->blocksize);
  }
}

void sampleBufferCopy(SampleBuffer self, const SampleBuffer buffer) {
  int i;
  if(self->blocksize != buffer->blocksize) {
    return;
  }
  else if(self->numChannels != buffer->numChannels) {
    return;
  }
  for(i = 0; i < self->numChannels; i++) {
    memcpy(self->samples[i], buffer->samples[i], sizeof(Sample) * self->blocksize);
  }
}

void freeSampleBuffer(SampleBuffer sampleBuffer) {
  int i;
  if(sampleBuffer == NULL) {
    return;
  }
  if(sampleBuffer->samples != NULL) {
    for(i = 0; i < 2; i++) {
      free(sampleBuffer->samples[i]);
    }
    free(sampleBuffer->samples);
  }
  free(sampleBuffer);
}
