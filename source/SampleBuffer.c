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
#include "SampleBuffer.h"

SampleBuffer newSampleBuffer(int numChannels, int blocksize) {
  SampleBuffer sampleBuffer = (SampleBuffer)malloc(sizeof(SampleBufferMembers));
  int i;

  sampleBuffer->numChannels = numChannels;
  sampleBuffer->blocksize = blocksize;
  sampleBuffer->samples = (Samples*)malloc(sizeof(Samples) * numChannels);
  for(i = 0; i < numChannels; i++) {
    sampleBuffer->samples[i] = (Samples)malloc(sizeof(Sample) * blocksize);
  }
  clearSampleBuffer(sampleBuffer);

  return sampleBuffer;
}

void clearSampleBuffer(SampleBuffer sampleBuffer) {
  int i;
  for(i = 0; i < sampleBuffer->numChannels; i++) {
    memset(sampleBuffer->samples[i], 0, sizeof(Sample) * sampleBuffer->blocksize);
  }
}

void copySampleBuffers(SampleBuffer destBuffer, const SampleBuffer srcBuffer) {
  int i;
  if(destBuffer->blocksize != srcBuffer->blocksize) {
    return;
  }
  else if(destBuffer->numChannels != srcBuffer->numChannels) {
    return;
  }
  for(i = 0; i < destBuffer->numChannels; i++) {
    memcpy(destBuffer->samples[i], srcBuffer->samples[i], sizeof(Sample) * destBuffer->blocksize);
  }
}

void freeSampleBuffer(SampleBuffer sampleBuffer) {
  int i;
  for(i = 0; i < sampleBuffer->numChannels; i++) {
    free(sampleBuffer->samples[i]);
  }
  free(sampleBuffer->samples);
  free(sampleBuffer);
}
