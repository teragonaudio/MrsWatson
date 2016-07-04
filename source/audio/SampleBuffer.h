//
// SampleBuffer.h - MrsWatson
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

#ifndef MrsWatson_SampleBuffer_h
#define MrsWatson_SampleBuffer_h

#include "base/Types.h"

typedef struct {
  ChannelCount numChannels;
  SampleCount blocksize;
  Samples *samples;
} SampleBufferMembers;
typedef SampleBufferMembers *SampleBuffer;

/**
 * Create a new SampleBuffer instance
 * @param numChannels Number of channels
 * @param blocksize Processing blocksize to use
 * @return An initialized SampleBuffer instance
 */
SampleBuffer newSampleBuffer(ChannelCount numChannels, SampleCount blocksize);

/**
 * Set all samples to zero
 * @param self
 */
void sampleBufferClear(SampleBuffer self);

/**
 * Copy some samples from another buffer to this one
 * @param destinationBuffer
 * @param destinationOffset zero-based index of where to start in
 * destinationBuffer.
 * @param sourceBuffer Other buffer to copy from
 * @param sourceOffset zero-based index of where to start in buffer.
 * @param numberOfFrames number of frames to copy.
 * @return True on success, false on failure
 */
boolByte sampleBufferCopyAndMapChannelsWithOffset(
    SampleBuffer destinationBuffer, SampleCount destinationOffset,
    const SampleBuffer sourceBuffer, SampleCount sourceOffset,
    SampleCount numberOfFrames);

/**
* Copy all samples from another buffer to this one
* @param self
* @param buffer Other buffer to copy from
* @return True on success, false on failure
*/
boolByte sampleBufferCopyAndMapChannels(SampleBuffer self,
                                        const SampleBuffer buffer);

/**
 * Free all memory used by a SampleBuffer instance
 * @param sampleBuffer
 */
void freeSampleBuffer(SampleBuffer self);

#endif
