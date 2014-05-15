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
#include "base/PlatformUtilities.h"
#include "logging/EventLogger.h"

SampleBuffer newSampleBuffer(unsigned int numChannels, unsigned long blocksize) {
  SampleBuffer sampleBuffer = NULL;
  unsigned int i;

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

boolByte sampleBufferCopyAndMapChannelsAdvanced(SampleBuffer destinationBuffer, unsigned long destinationOffset, const SampleBuffer sourceBuffer, unsigned long sourceOffset, unsigned long numberOfSamples) {
  unsigned int i;

  // Definitely not supported.
  if(destinationBuffer->blocksize < destinationOffset + numberOfSamples) {
    logInternalError("Destination buffer size %d < %d", destinationBuffer->blocksize, destinationOffset + numberOfSamples);
    return false;
  }
  // Definitely not supported.
  if(sourceBuffer->blocksize < sourceOffset + numberOfSamples) {
    logInternalError("Source buffer size %d < %d", sourceBuffer->blocksize, sourceOffset + numberOfSamples);
    return false;
  }

  if(sourceBuffer->numChannels != destinationBuffer->numChannels) {
    logDebug("Mapping channels from %d -> %d", sourceBuffer->numChannels, destinationBuffer->numChannels);
  }

  // If the other buffer is bigger (or the same size) as this buffer, then only
  // copy up to the channel count of this buffer. Any other data will be lost,
  // sorry about that!
  if(sourceBuffer->numChannels >= destinationBuffer->numChannels) {
    for(i = 0; i < destinationBuffer->numChannels; i++) {
      memcpy(destinationBuffer->samples[i]+destinationOffset, sourceBuffer->samples[i]+sourceOffset, sizeof(Sample) * numberOfSamples);
    }
  }
  // But if this buffer is bigger than the other buffer, then copy all channels
  // to this one. For example, if this buffer is 4 channels and the other buffer
  // is 2 channels, then we copy the stereo pair to this channel (L R L R).
  else {
    for(i = 0; i < destinationBuffer->numChannels; i++) {
      if(sourceBuffer->numChannels > 0) {
        memcpy(destinationBuffer->samples[i]+destinationOffset, sourceBuffer->samples[i % sourceBuffer->numChannels]+sourceOffset, sizeof(Sample) * numberOfSamples);
      } else { // If the other buffer has zero channels just clear this buffer.
        memset(destinationBuffer->samples[i]+destinationOffset, 0, sizeof(Sample) * numberOfSamples);
      }
    }
  }

  return true;
}

boolByte sampleBufferCopyAndMapChannels(SampleBuffer self, const SampleBuffer buffer) {
  // Definitely not supported, otherwise it would be hard to deal with partial
  // copies and so forth.
  if(self->blocksize != buffer->blocksize) {
    logInternalError("Source and destination buffer are not the same size");
    return false;
  }
  return sampleBufferCopyAndMapChannelsAdvanced(self, 0, buffer, 0, self->blocksize);
}

void sampleBufferCopyPcmSamples(SampleBuffer self, const short* inPcmSamples) {
  const unsigned int numChannels = self->numChannels;
  const unsigned long numInterlacedSamples = numChannels * self->blocksize;
  unsigned int currentInterlacedSample = 0;
  unsigned int currentDeinterlacedSample = 0;
  unsigned int currentChannel;

  while(currentInterlacedSample < numInterlacedSamples) {
    for(currentChannel = 0; currentChannel < numChannels; ++currentChannel) {
      Sample convertedSample = (Sample)inPcmSamples[currentInterlacedSample++] / 32767.0f;
      self->samples[currentChannel][currentDeinterlacedSample] = convertedSample;
    }
    ++currentDeinterlacedSample;
  }
}

void sampleBufferGetPcmSamples(const SampleBuffer self, short* outPcmSamples, boolByte flipEndian) {
  const unsigned long blocksize = self->blocksize;
  const unsigned int numChannels = self->numChannels;
  unsigned int currentInterlacedSample = 0;
  unsigned int currentSample = 0;
  unsigned int currentChannel = 0;
  short shortValue;
  Sample sample;

  for(currentSample = 0; currentSample < blocksize; ++currentSample) {
    for(currentChannel = 0; currentChannel < numChannels; ++currentChannel) {
      sample = self->samples[currentChannel][currentSample];
      shortValue = (short)(sample * 32767.0f);
      if(flipEndian) {
        outPcmSamples[currentInterlacedSample++] = flipShortEndian(shortValue);
      }
      else {
        outPcmSamples[currentInterlacedSample++] = shortValue;
      }
    }
  }
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
