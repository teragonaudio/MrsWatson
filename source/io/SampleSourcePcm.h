//
// SampleSourcePcm.h - MrsWatson
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

#ifndef MrsWatson_InputSourcePcm_h
#define MrsWatson_InputSourcePcm_h

#include "audio/PcmSampleBuffer.h"
#include "io/SampleSource.h"

#include <stdio.h>

typedef struct {
  boolByte isStream;
  boolByte isLittleEndian;
  FILE *fileHandle;
  size_t dataBufferNumItems;
  PcmSampleBuffer pcmSampleBuffer;

  ChannelCount numChannels;
  SampleRate sampleRate;
  BitDepth bitDepth;
} SampleSourcePcmDataMembers;
typedef SampleSourcePcmDataMembers *SampleSourcePcmData;

/**
 * Read raw PCM data to a floating-point sample buffer
 * @param self
 * @param sampleBuffer
 * @return Number of samples read
 */
SampleCount sampleSourcePcmRead(SampleSourcePcmData extraData,
                                SampleBuffer sampleBuffer);

/**
 * Writes data from a sample buffer to a PCM output
 * @param self
 * @param sampleBuffer
 * @return Number of samples written
 */
SampleCount sampleSourcePcmWrite(SampleSourcePcmData extraData,
                                 const SampleBuffer sampleBuffer);

/**
 * Set the sample rate to be used for raw PCM file operations. This is most
 * relevant when writing a WAVE or a AIFF file, as the sample rate must be given
 * in the file header.
 * @param sampleSourcePtr
 * @param sampleRate Sample rate, in Hertz
 */
void sampleSourcePcmSetSampleRate(void *selfPtr, double sampleRate);

/**
 * Set the number of channels to be used for raw PCM file operations. Like the
 * sample, this is most relevant when writing a WAVE or a AIFF file.
 * @param sampleSourcePtr
 * @param numChannels Number of channels
 */
void sampleSourcePcmSetNumChannels(void *selfPtr, int numChannels);

/**
 * Free a PCM sample source and all associated data
 * @param sampleSourceDataPtr Pointer to sample source data
 */
void freeSampleSourceDataPcm(void *extraDataPtr);

#endif
