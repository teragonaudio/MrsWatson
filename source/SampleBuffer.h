//
//  SampleBuffer.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#ifndef MrsWatson_SampleBuffer_h
#define MrsWatson_SampleBuffer_h

#define DEFAULT_BLOCKSIZE 512

typedef float Sample;
typedef Sample* Samples;

typedef struct {
  int numChannels;
  int blocksize;
  Samples samples;
} SampleBufferMembers;

typedef SampleBufferMembers* SampleBuffer;

SampleBuffer newSampleBuffer(int numChannels, int blocksize);
void freeSampleBuffer(SampleBuffer sampleBuffer);

#endif
