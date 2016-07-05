//
// PcmSampleBuffer.h - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
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

#ifndef MrsWatson_PcmSampleBuffer_h
#define MrsWatson_PcmSampleBuffer_h

#include "audio/AudioSettings.h"
#include "audio/SampleBuffer.h"

typedef SampleBuffer (*PcmSampleBufferGetSampleBufferFunc)(void *selfPtr);

typedef void (*PcmSampleBufferSetSampleBufferFunc)(void *selfPtr,
                                                   SampleBuffer sampleBuffer);

typedef void (*PcmSampleBufferSetSamplesFunc)(void *selfPtr);

typedef struct {
  void *pcmSamples;
  BitDepth bitDepth;
  boolByte littleEndian;
  SampleCount bytesPerSample;

  PcmSampleBufferGetSampleBufferFunc getSampleBuffer;
  PcmSampleBufferSetSampleBufferFunc setSampleBuffer;
  PcmSampleBufferSetSamplesFunc setSamples;

  SampleBuffer _super;
} PcmSampleBufferMembers;
typedef PcmSampleBufferMembers *PcmSampleBuffer;

PcmSampleBuffer newPcmSampleBuffer(ChannelCount numChannels,
                                   SampleCount blocksize, BitDepth bitDepth);

void freePcmSampleBuffer(PcmSampleBuffer self);

#endif
