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
#include <math.h>

#include "audio/AudioSettings.h"
#include "audio/SampleBuffer.h"
#include "base/Endian.h"
#include "logging/EventLogger.h"
#include "SampleBuffer.h"

SampleBuffer newSampleBuffer(ChannelCount numChannels, SampleCount blocksize)
{
    SampleBuffer sampleBuffer = (SampleBuffer)malloc(sizeof(SampleBufferMembers));
    sampleBuffer->numChannels = numChannels;
    sampleBuffer->blocksize = blocksize;
    sampleBuffer->samples = (Samples *)malloc(sizeof(Samples) * numChannels);

    for (ChannelCount i = 0; i < numChannels; i++) {
        sampleBuffer->samples[i] = (Samples)malloc(sizeof(Sample) * blocksize);
    }

    sampleBufferClear(sampleBuffer);
    return sampleBuffer;
}

void sampleBufferClear(SampleBuffer self)
{
    for (ChannelCount i = 0; i < self->numChannels; i++) {
        memset(self->samples[i], 0, sizeof(Sample) * self->blocksize);
    }
}

boolByte sampleBufferCopyAndMapChannelsWithOffset(SampleBuffer destinationBuffer,
                                                  SampleCount destinationOffset,
                                                  const SampleBuffer sourceBuffer,
                                                  SampleCount sourceOffset,
                                                  SampleCount numberOfFrames)
{
    // Definitely not supported.
    if (destinationBuffer->blocksize < destinationOffset + numberOfFrames) {
        logInternalError("Destination buffer size %d < %d", destinationBuffer->blocksize, destinationOffset + numberOfFrames);
        return false;
    }

    // Definitely not supported.
    if (sourceBuffer->blocksize < sourceOffset + numberOfFrames) {
        logInternalError("Source buffer size %d < %d", sourceBuffer->blocksize, sourceOffset + numberOfFrames);
        return false;
    }

    if (sourceBuffer->numChannels != destinationBuffer->numChannels) {
        logDebug("Mapping channels from %d -> %d", sourceBuffer->numChannels, destinationBuffer->numChannels);
    }

    // If the other buffer is bigger (or the same size) as this buffer, then only
    // copy up to the channel count of this buffer. Any other data will be lost,
    // sorry about that!
    if (sourceBuffer->numChannels >= destinationBuffer->numChannels) {
        for (ChannelCount i = 0; i < destinationBuffer->numChannels; ++i) {
            memcpy(destinationBuffer->samples[i] + destinationOffset, sourceBuffer->samples[i] + sourceOffset, sizeof(Sample) * numberOfFrames);
        }
    }
    // But if this buffer is bigger than the other buffer, then copy all channels
    // to this one. For example, if this buffer is 4 channels and the other buffer
    // is 2 channels, then we copy the stereo pair to this channel (L R L R).
    else {
        for (ChannelCount i = 0; i < destinationBuffer->numChannels; ++i) {
            if (sourceBuffer->numChannels > 0) {
                memcpy(destinationBuffer->samples[i] + destinationOffset,
                       sourceBuffer->samples[i % sourceBuffer->numChannels] + sourceOffset,
                       sizeof(Sample) * numberOfFrames);
            } else {
                // If the other buffer has zero channels just clear this buffer.
                memset(destinationBuffer->samples[i] + destinationOffset, 0, sizeof(Sample) * numberOfFrames);
            }
        }
    }

    return true;
}

boolByte sampleBufferCopyAndMapChannels(SampleBuffer self, const SampleBuffer buffer)
{
    // Definitely not supported, otherwise it would be hard to deal with partial
    // copies and so forth.
    if (self->blocksize != buffer->blocksize) {
        logInternalError("Source and destination buffer are not the same size");
        return false;
    }

    return sampleBufferCopyAndMapChannelsWithOffset(self, 0, buffer, 0, self->blocksize);
}

void freeSampleBuffer(SampleBuffer sampleBuffer)
{
    if (self == NULL) {
        return;
    }

    for (ChannelCount channel = 0; channel < self->numChannels; ++channel) {
        free(self->samples[channel]);
    }
    free(self->samples);
    free(self);
}
