//
//  SampleBuffer.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#import <string.h>
#include "SampleBuffer.h"

SampleBuffer newSampleBuffer(int numChannels, int blocksize) {
  SampleBuffer sampleBuffer = malloc(sizeof(SampleBufferMembers));

  sampleBuffer->numChannels = numChannels;
  sampleBuffer->blocksize = blocksize;
  sampleBuffer->samples = malloc(sizeof(Samples) * numChannels);
  for(int i = 0; i < numChannels; i++) {
    sampleBuffer->samples[i] = malloc(sizeof(Sample) * blocksize);
  }
  clearSampleBuffer(sampleBuffer);

  return sampleBuffer;
}

void clearSampleBuffer(SampleBuffer sampleBuffer) {
  for(int i = 0; i < sampleBuffer->numChannels; i++) {
    memset(sampleBuffer->samples[i], 0, sizeof(Sample) * sampleBuffer->blocksize);
  }
}

void copySampleBuffers(SampleBuffer destBuffer, const SampleBuffer srcBuffer) {
  for(int i = 0; i < destBuffer->numChannels; i++) {
    memcpy(destBuffer->samples[i], srcBuffer->samples[i], sizeof(Sample) * destBuffer->blocksize);
  }
}

void freeSampleBuffer(SampleBuffer sampleBuffer) {
  for(int i = 0; i < sampleBuffer->numChannels; i++) {
    free(sampleBuffer->samples[i]);
  }
  free(sampleBuffer->samples);
  free(sampleBuffer);
}