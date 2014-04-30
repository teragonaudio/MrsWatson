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
#include <stdio.h>

typedef float Sample;
typedef Sample* Samples;

typedef struct {
  unsigned int numChannels;
  size_t blocksize;
  Samples* samples;
} SampleBufferMembers;
typedef SampleBufferMembers* SampleBuffer;

/**
 * Create a new SampleBuffer instance
 * @param numChannels Number of channels
 * @param blocksize Processing blocksize to use
 * @return An initialized SampleBuffer instance
 */
SampleBuffer newSampleBuffer(unsigned int numChannels, unsigned long blocksize);

/**
 * Set all samples to zero
 * @param self
 */
void sampleBufferClear(SampleBuffer self);

/**
 * Expand or shrink the channel count of a sample buffer. Useful for copying
 * between stereo/mono and such.
 * @param self
 * @param numChannels New channel count
 * @param copy Clone data from channel 0 to new channels if expanding the SampleBuffer
 * @return True on success, false on failure
 */
boolByte sampleBufferResize(SampleBuffer self, const unsigned int numChannels, boolByte copy);

/**
 * Copy all samples from another buffer to this one
 * @param self
 * @param buffer Other buffer to copy from
 * @return True on success, false on failure
 */
boolByte sampleBufferCopy(SampleBuffer self, const SampleBuffer buffer);

/**
 * Copy a buffer of interlaced short integer samples to a sample buffer. This
 * function also converts the samples from integers to floating-point numbers.
 * Mostly useful for reading raw PCM data into a format usable by plugins.
 * @param self
 * @param inPcmSamples Array of interlaced samples. Note that the size of the
 * length of this array must match the SampleBuffer's blocksize * channel count,
 * or else undefined behavior will occur.
 */
void sampleBufferCopyPcmSamples(SampleBuffer self, const short* inPcmSamples);

/**
 * Get an array of interlaced short integer samples from the SampleBuffer. This
 * function will also convert the samples from floating-point numbers to short
 * integers. Mostly useful for writing raw PCM data.
 * @param self
 * @param outPcmSamples A pre-allocated array large enough to hold the result of
 * the conversion. This means that at least blocksize * channel count samples
 * must be allocated
 * @param flipEndian True if the output data should have the samples flipped
 * from the native endianness.
 */
void sampleBufferGetPcmSamples(const SampleBuffer self, short* outPcmSamples, boolByte flipEndian);

/**
 * Free all memory used by a SampleBuffer instance
 * @param sampleBuffer
 */
void freeSampleBuffer(SampleBuffer sampleBuffer);

#endif
