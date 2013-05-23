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

SampleBuffer newSampleBuffer(unsigned int numChannels, unsigned long blocksize) {
  SampleBuffer sampleBuffer = NULL;
  unsigned int i;

  if(numChannels <= 0) {
    logError("Cannot create sample buffer with channel count %d", numChannels);
    return NULL;
  }
  if(blocksize <= 0) {
    logError("Cannot create sample buffer with blocksize %d", blocksize);
    return NULL;
  }

  sampleBuffer = (SampleBuffer)malloc(sizeof(SampleBufferMembers));
  sampleBuffer->numChannels = numChannels;
  sampleBuffer->blocksize = blocksize;

  sampleBuffer->samples = (Samples*)malloc(sizeof(Samples) * numChannels);
  for(i = 0; i < numChannels; i++) {
    sampleBuffer->samples[i] = (Samples)malloc(sizeof(Sample) * blocksize);
  }
  sampleBufferClear(sampleBuffer);

  return sampleBuffer;
}

void sampleBufferClear(SampleBuffer self) {
  unsigned int i;
  for(i = 0; i < self->numChannels; i++) {
    memset(self->samples[i], 0, sizeof(Sample) * self->blocksize);
  }
}

boolByte sampleBufferCopy(SampleBuffer self, const SampleBuffer buffer) {
  unsigned int i;

  // Definitely not supported, otherwise it would be hard to deal with partial
  // copies and so forth.
  if(self->blocksize != buffer->blocksize) {
    return false;
  }

  // If the other buffer is bigger (or the same size) as this buffer, then only
  // copy up to the channel count of this buffer. Any other data will be lost,
  // sorry about that!
  if(buffer->numChannels >= self->numChannels) {
    for(i = 0; i < self->numChannels; i++) {
      memcpy(self->samples[i], buffer->samples[i], sizeof(Sample) * self->blocksize);
    }
  }
  // But if this buffer is bigger than the other buffer, then copy all channels
  // to this one. For example, if this buffer is 4 channels and the other buffer
  // is 2 channels, then we copy the stereo pair to this channel (L R L R).
  else {
    for(i = 0; i < self->numChannels; i++) {
      memcpy(self->samples[i], buffer->samples[i % buffer->numChannels], sizeof(Sample) * self->blocksize);
    }
  }

  return true;
}

boolByte sampleBufferResize(SampleBuffer self, const unsigned int numChannels, boolByte copy) {
  unsigned int i;

  if(numChannels == self->numChannels || numChannels == 0) {
    return false;
  }
  else if(numChannels < self->numChannels) {
    for(i = self->numChannels - 1; i >= numChannels; i--) {
      free(self->samples[i]);
    }
    self->numChannels = numChannels;
    return true;
  }
  else if(numChannels > self->numChannels) {
    self->samples = realloc(self->samples, sizeof(Samples) * numChannels);
    for(i = self->numChannels; i < numChannels; i++) {
      self->samples[i] = (Samples)malloc(sizeof(Sample) * self->blocksize);
      if(copy) {
        memcpy(self->samples[i], self->samples[0], sizeof(Sample) * self->blocksize);
      }
      else {
        memset(self->samples[i], 0, sizeof(Sample) * self->blocksize);
      }
    }
    self->numChannels = numChannels;
    return true;
  }
  else {
    // Invalid state, probably shouldn't happen...
  }

  return false;
}

void freeSampleBuffer(SampleBuffer sampleBuffer) {
  unsigned int i;
  if(sampleBuffer == NULL) {
    return;
  }
  if(sampleBuffer->samples != NULL) {
    for(i = 0; i < sampleBuffer->numChannels; i++) {
      free(sampleBuffer->samples[i]);
    }
    free(sampleBuffer->samples);
  }
  free(sampleBuffer);
}
