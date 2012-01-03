//
//  SampleBuffer.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "SampleBuffer.h"

SampleBuffer newSampleBuffer(int numChannels, int blocksize) {
  SampleBuffer sampleBuffer = malloc(sizeof(SampleBufferMembers));

  sampleBuffer->numChannels = numChannels;
  sampleBuffer->blocksize = blocksize;
  sampleBuffer->samples = malloc(sizeof(Samples) * numChannels);
  for(int i = 0; i < numChannels; i++) {
    sampleBuffer->samples[i] = malloc(sizeof(Sample) * blocksize);
  }

  return sampleBuffer;
}

void freeSampleBuffer(SampleBuffer sampleBuffer) {
  for(int i = 0; i < sampleBuffer->numChannels; i++) {
    free(sampleBuffer->samples[i]);
  }
  free(sampleBuffer->samples);
  free(sampleBuffer);
}