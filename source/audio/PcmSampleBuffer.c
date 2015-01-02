//
// PcmSampleBuffer.c - MrsWatson
// Created by Nik Reiman on 16 Dec 14.
// Copyright (c) 2014 Teragon Audio. All rights reserved.
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

#include <math.h>
#include <string.h>

#include "audio/PcmSampleBuffer.h"
#include "base/Endian.h"
#include "base/PlatformInfo.h"

static SampleBuffer _getSampleBuffer(void *selfPtr)
{
    PcmSampleBuffer self = (PcmSampleBuffer)selfPtr;
    return self->_super;
}

// Normally we'd return `Sample` here, however for the 32-bit functions that
// would cause an overflow for 32-bit floating point values.
static double _getMaxPcmSampleValue(const PcmSampleBuffer self)
{
    return pow(2.0, (double)(self->bitDepth - 1)) - 1.0;
}

static void _setSampleBuffer8Bit(void *selfPtr, SampleBuffer sampleBuffer)
{
    PcmSampleBuffer self = (PcmSampleBuffer)selfPtr;
    const double pcmSampleMax = _getMaxPcmSampleValue(self);
    unsigned char *charSamples = (unsigned char *)(self->pcmSamples);
    SampleCount index = 0;

    // 8-bit PCM samples are unsigned, so instead of relying on 2's compliment
    // storage, we must map the samples from {-1.0 .. 1.0} - {0 .. 255}
    for (SampleCount sample = 0; sample < sampleBuffer->blocksize; ++sample) {
        for (ChannelCount channel = 0; channel < sampleBuffer->numChannels; ++channel) {
            charSamples[index++] = (unsigned char)((sampleBuffer->samples[channel][sample] + 1.0f) * pcmSampleMax);
        }
    }
}

static void _setSampleBuffer16Bit(void *selfPtr, SampleBuffer sampleBuffer)
{
    PcmSampleBuffer self = (PcmSampleBuffer)selfPtr;
    const double pcmSampleMax = _getMaxPcmSampleValue(self);
    short *shortSamples = (short *)(self->pcmSamples);
    SampleCount index = 0;

    for (SampleCount sample = 0; sample < sampleBuffer->blocksize; ++sample) {
        for (ChannelCount channel = 0; channel < sampleBuffer->numChannels; ++channel) {
            shortSamples[index++] = (short)(sampleBuffer->samples[channel][sample] * pcmSampleMax);
        }
    }
}

static void _setSampleBuffer24Bit(void *selfPtr, SampleBuffer sampleBuffer)
{
    PcmSampleBuffer self = (PcmSampleBuffer)selfPtr;
    const double pcmSampleMax = _getMaxPcmSampleValue(self);
    int *intSamples = (int *)(self->pcmSamples);
    SampleCount index = 0;

    for (SampleCount sample = 0; sample < sampleBuffer->blocksize; ++sample) {
        for (ChannelCount channel = 0; channel < sampleBuffer->numChannels; ++channel) {
            intSamples[index++] = (int)(sampleBuffer->samples[channel][sample] * pcmSampleMax);
        }
    }
}

static void _setSampleBuffer32Bit(void *selfPtr, SampleBuffer sampleBuffer)
{
    PcmSampleBuffer self = (PcmSampleBuffer)selfPtr;
    const double pcmSampleMax = _getMaxPcmSampleValue(self);
    int *intSamples = (int *)(self->pcmSamples);
    SampleCount index = 0;

    for (SampleCount sample = 0; sample < sampleBuffer->blocksize; ++sample) {
        for (ChannelCount channel = 0; channel < sampleBuffer->numChannels; ++channel) {
            intSamples[index++] = (int)(sampleBuffer->samples[channel][sample] * pcmSampleMax);
        }
    }
}

static void _setSamples8Bit(void *selfPtr)
{
    PcmSampleBuffer self = (PcmSampleBuffer)selfPtr;
    Samples *samples = self->_super->samples;
    unsigned char *charSamples = (unsigned char *)(self->pcmSamples);
    const SampleCount numSamples = self->_super->blocksize * self->_super->numChannels;
    SampleCount sampleIndex = 0;
    ChannelCount channelIndex = 0;
    const double pcmSampleMax = _getMaxPcmSampleValue(self);

    for (SampleCount sample = 0; sample < numSamples; ++sample) {
        samples[channelIndex++][sampleIndex] = (Sample)((double)(charSamples[sample] - 127) / pcmSampleMax);
        if (channelIndex >= self->_super->numChannels) {
            channelIndex = 0;
            ++sampleIndex;
        }
    }
}

static void _setSamples16Bit(void *selfPtr)
{
    PcmSampleBuffer self = (PcmSampleBuffer)selfPtr;
    Samples *samples = self->_super->samples;
    short *shortSamples = (short *)(self->pcmSamples);
    const SampleCount numSamples = self->_super->blocksize * self->_super->numChannels;
    SampleCount sampleIndex = 0;
    ChannelCount channelIndex = 0;
    const double pcmSampleMax = _getMaxPcmSampleValue(self);

    if(platformInfoIsLittleEndian() && self->littleEndian) {
        for (SampleCount sample = 0; sample < numSamples; ++sample) {
            samples[channelIndex++][sampleIndex] = (Sample)((double)shortSamples[sample] / pcmSampleMax);
            if (channelIndex >= self->_super->numChannels) {
                channelIndex = 0;
                ++sampleIndex;
            }
        }
    } else {
        short flippedSample;
        for (SampleCount sample = 0; sample < numSamples; ++sample) {
            flippedSample = flipShortEndian(shortSamples[sample]);
            samples[channelIndex++][sampleIndex] = (Sample)((double)flippedSample / pcmSampleMax);
            if (channelIndex >= self->_super->numChannels) {
                channelIndex = 0;
                ++sampleIndex;
            }
        }
    }
}

static void _setSamples24Bit(void *selfPtr)
{
    PcmSampleBuffer self = (PcmSampleBuffer)selfPtr;
    Samples *samples = self->_super->samples;
    char *charSamples = (char *)(self->pcmSamples);
    const SampleCount numSamples = self->_super->blocksize * self->_super->numChannels;
    SampleCount sampleIndex = 0;
    ChannelCount channelIndex = 0;
    const double pcmSampleMax = _getMaxPcmSampleValue(self);
    int value;

    if(self->littleEndian) {
        for (size_t index = 0; index < numSamples * self->bytesPerSample; index += 3) {
            // If the topmost bit here is set, then we have a negative number and must
            // take the 2's compliment of it.
            if (charSamples[index + 2] & 0x80) {
                value = 0xffffffff;
                value &= (charSamples[index + 2] << 16) | 0xff000000;
                value &= (charSamples[index + 1] << 8) | 0xffff0000;
                value &= (charSamples[index]) | 0xffffff00;
            } else {
                value = 0;
                value |= (charSamples[index + 2] << 16) & 0x00ff0000;
                value |= (charSamples[index + 1] << 8) & 0x0000ff00;
                value |= (charSamples[index]) & 0x000000ff;
            }

            samples[channelIndex++][sampleIndex] = (Sample)((double)value / pcmSampleMax);
            if (channelIndex >= self->_super->numChannels) {
                channelIndex = 0;
                ++sampleIndex;
            }
        }
    } else {
        for (size_t index = 0; index < numSamples * self->bytesPerSample; index += 3) {
            // If the topmost bit here is set, then we have a negative number and must
            // take the 2's compliment of it.
            if (charSamples[index] & 0x80) {
                value = 0xffffffff;
                value &= (charSamples[index] << 16) | 0xff000000;
                value &= (charSamples[index + 1] << 8) | 0xffff0000;
                value &= (charSamples[index + 2]) | 0xffffff00;
            } else {
                value = 0;
                value |= (charSamples[index] << 16) & 0x00ff0000;
                value |= (charSamples[index + 1] << 8) & 0x0000ff00;
                value |= (charSamples[index + 2]) & 0x000000ff;
            }

            samples[channelIndex++][sampleIndex] = (Sample)((double)value / pcmSampleMax);
            if (channelIndex >= self->_super->numChannels) {
                channelIndex = 0;
                ++sampleIndex;
            }
        }
    }
}

static void _setSamples32Bit(void *selfPtr)
{
    PcmSampleBuffer self = (PcmSampleBuffer)selfPtr;
    Samples *samples = self->_super->samples;
    // 32-bit PCM files are actually not stored as 32-bit integer data,
    // but rather 32-bit floats written directly to disk. I guess this
    // is meant to be more efficient since on matching endian platforms
    // byte-flipping is not necessary.
    float *floatSamples = (float *)(self->pcmSamples);
    const SampleCount numSamples = self->_super->blocksize * self->_super->numChannels;
    SampleCount sampleIndex = 0;
    ChannelCount channelIndex = 0;

    if (platformInfoIsLittleEndian() && self->littleEndian) {
        for (SampleCount sample = 0; sample < numSamples; ++sample) {
            samples[channelIndex++][sampleIndex] = floatSamples[sample];
            if (channelIndex >= self->_super->numChannels) {
                channelIndex = 0;
                ++sampleIndex;
            }
        }
    } else {
        float flippedSample;
        for (SampleCount sample = 0; sample < numSamples; ++sample) {
            flippedSample = convertBigEndianFloatToPlatform(floatSamples[sample]);
            samples[channelIndex++][sampleIndex] = flippedSample;
            if (channelIndex >= self->_super->numChannels) {
                channelIndex = 0;
                ++sampleIndex;
            }
        }
    }
}

PcmSampleBuffer newPcmSampleBuffer(ChannelCount numChannels, SampleCount blocksize, BitDepth bitDepth)
{
    PcmSampleBuffer pcmSampleBuffer = (PcmSampleBuffer)malloc(sizeof(PcmSampleBufferMembers));

    pcmSampleBuffer->littleEndian = true;
    pcmSampleBuffer->bitDepth = bitDepth;
    pcmSampleBuffer->bytesPerSample = bitDepth / 8;
    pcmSampleBuffer->pcmSamples = malloc(numChannels * blocksize * pcmSampleBuffer->bytesPerSample);
    pcmSampleBuffer->getSampleBuffer = _getSampleBuffer;

    switch (bitDepth) {
        case kBitDepth8Bit:
            pcmSampleBuffer->setSampleBuffer = _setSampleBuffer8Bit;
            pcmSampleBuffer->setSamples = _setSamples8Bit;
            break;
        case kBitDepth16Bit:
            pcmSampleBuffer->setSampleBuffer = _setSampleBuffer16Bit;
            pcmSampleBuffer->setSamples = _setSamples16Bit;
            break;
        case kBitDepth24Bit:
            pcmSampleBuffer->setSampleBuffer = _setSampleBuffer24Bit;
            pcmSampleBuffer->setSamples = _setSamples24Bit;
            break;
        case kBitDepth32Bit:
            pcmSampleBuffer->setSampleBuffer = _setSampleBuffer32Bit;
            pcmSampleBuffer->setSamples = _setSamples32Bit;
            break;
    }

    pcmSampleBuffer->_super = newSampleBuffer(numChannels, blocksize);
    return pcmSampleBuffer;
}

void freePcmSampleBuffer(PcmSampleBuffer self) {
    if (self != NULL) {
        freeSampleBuffer(self->_super);
        if (self->pcmSamples != NULL) {
            free(self->pcmSamples);
        }
        free(self);
    }
}
