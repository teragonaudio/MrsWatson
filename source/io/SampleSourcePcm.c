//
// SampleSourcePcm.c - MrsWatson
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

#include "audio/AudioSettings.h"
#include "audio/PcmSampleBuffer.h"
#include "io/SampleSourcePcm.h"
#include "logging/EventLogger.h"

static boolByte openSampleSourcePcm(void *selfPtr, const SampleSourceOpenAs openAs)
{
    SampleSource self = (SampleSource) selfPtr;
    SampleSourcePcmData extraData = (SampleSourcePcmData)(self->extraData);

    if (openAs == SAMPLE_SOURCE_OPEN_READ) {
        if (charStringIsEqualToCString(self->sourceName, "-", false)) {
            extraData->fileHandle = stdin;
            charStringCopyCString(self->sourceName, "stdin");
            extraData->isStream = true;
        } else {
            extraData->fileHandle = fopen(self->sourceName->data, "rb");
        }
    } else if (openAs == SAMPLE_SOURCE_OPEN_WRITE) {
        if (charStringIsEqualToCString(self->sourceName, "-", false)) {
            extraData->fileHandle = stdout;
            charStringCopyCString(self->sourceName, "stdout");
            extraData->isStream = true;
        } else {
            extraData->fileHandle = fopen(self->sourceName->data, "wb");
        }
    } else {
        logInternalError("Invalid type for openAs in PCM file");
        return false;
    }

    if (extraData->fileHandle == NULL) {
        logError("PCM File '%s' could not be opened for %s",
                 self->sourceName->data, openAs == SAMPLE_SOURCE_OPEN_READ ? "reading" : "writing");
        return false;
    }

    self->openedAs = openAs;
    return true;
}

size_t sampleSourcePcmRead(SampleSourcePcmData extraData, SampleBuffer sampleBuffer)
{
    if (extraData == NULL || extraData->fileHandle == NULL) {
        logCritical("Corrupt PCM data structure");
        return 0;
    }

    // If the blocksize has changed, then regenerate our PCM sample buffer to
    // make room for it.
    const SampleBuffer internalSampleBuffer = extraData->pcmSampleBuffer->getSampleBuffer(extraData->pcmSampleBuffer);
    if (internalSampleBuffer->blocksize != sampleBuffer->blocksize ||
            internalSampleBuffer->numChannels != sampleBuffer->numChannels) {
        freePcmSampleBuffer(extraData->pcmSampleBuffer);
        extraData->pcmSampleBuffer = newPcmSampleBuffer(sampleBuffer->numChannels, sampleBuffer->blocksize, getBitDepth());
        extraData->dataBufferNumItems = sampleBuffer->numChannels * sampleBuffer->blocksize;
    }

    // Read data into our temporary holding buffer, and then set it to the
    // PcmSampleBuffer, which will convert it to floating point for us.
    size_t pcmSamplesRead = fread(extraData->pcmSampleBuffer->pcmSamples,
                                  extraData->pcmSampleBuffer->bytesPerSample,
                                  extraData->dataBufferNumItems,
                                  extraData->fileHandle);
    extraData->pcmSampleBuffer->setSamples(extraData->pcmSampleBuffer);
    sampleBufferCopyAndMapChannels(sampleBuffer, extraData->pcmSampleBuffer->getSampleBuffer(extraData->pcmSampleBuffer));

    if (pcmSamplesRead < extraData->dataBufferNumItems) {
        logDebug("End of PCM file reached");
        // Set the blocksize of the sample buffer to be the number of frames read
        sampleBuffer->blocksize = pcmSamplesRead / sampleBuffer->numChannels;
    }

    logDebug("Read %d samples from PCM file", pcmSamplesRead);
    return pcmSamplesRead;
}

static boolByte readBlockFromPcmFile(void *selfPtr, SampleBuffer sampleBuffer)
{
    SampleSource self = (SampleSource) selfPtr;
    SampleSourcePcmData extraData = (SampleSourcePcmData)(self->extraData);
    SampleCount originalBlocksize = sampleBuffer->blocksize;
    size_t samplesRead = sampleSourcePcmRead(extraData, sampleBuffer);
    self->numSamplesProcessed += samplesRead;
    return (boolByte)(originalBlocksize == sampleBuffer->blocksize);
}

size_t sampleSourcePcmWrite(SampleSourcePcmData extraData, const SampleBuffer sampleBuffer)
{
    size_t pcmSamplesWritten = 0;
    size_t numSamplesToWrite = (size_t)(sampleBuffer->numChannels * sampleBuffer->blocksize);

    if (extraData == NULL || extraData->fileHandle == NULL) {
        logCritical("Corrupt PCM data structure");
        return false;
    }

    extraData->pcmSampleBuffer->setSampleBuffer(extraData->pcmSampleBuffer, sampleBuffer);
    pcmSamplesWritten = fwrite(extraData->pcmSampleBuffer->pcmSamples, extraData->pcmSampleBuffer->bytesPerSample,
                               numSamplesToWrite, extraData->fileHandle);

    if (pcmSamplesWritten < numSamplesToWrite) {
        logWarn("Short write to PCM file");
        return pcmSamplesWritten;
    }

    logDebug("Wrote %d samples to PCM file", pcmSamplesWritten);
    return pcmSamplesWritten;
}

static boolByte writeBlockToPcmFile(void *selfPtr, const SampleBuffer sampleBuffer)
{
    SampleSource self = (SampleSource) selfPtr;
    SampleSourcePcmData extraData = (SampleSourcePcmData)(self->extraData);
    unsigned int samplesWritten = (int)sampleSourcePcmWrite(extraData, sampleBuffer);
    self->numSamplesProcessed += samplesWritten;
    return (boolByte)(samplesWritten == sampleBuffer->blocksize);
}

static void _closeSampleSourcePcm(void *selfPtr)
{
    SampleSource self = (SampleSource) selfPtr;
    SampleSourcePcmData extraData = (SampleSourcePcmData)self->extraData;

    if (extraData->fileHandle != NULL) {
        fclose(extraData->fileHandle);
    }
}

void sampleSourcePcmSetSampleRate(void *selfPtr, SampleRate sampleRate)
{
    SampleSource self = (SampleSource) selfPtr;
    SampleSourcePcmData extraData = (SampleSourcePcmData)self->extraData;
    extraData->sampleRate = (unsigned int)sampleRate;
}

void sampleSourcePcmSetNumChannels(void *selfPtr, int numChannels)
{
    SampleSource self = (SampleSource) selfPtr;
    SampleSourcePcmData extraData = (SampleSourcePcmData)self->extraData;
    extraData->numChannels = (unsigned short)numChannels;
}

void freeSampleSourceDataPcm(void *extraDataPtr)
{
    SampleSourcePcmData extraData = (SampleSourcePcmData) extraDataPtr;
    freePcmSampleBuffer(extraData->pcmSampleBuffer);
    free(extraData);
}

SampleSource _newSampleSourcePcm(const CharString sampleSourceName)
{
    SampleSource sampleSource = (SampleSource)malloc(sizeof(SampleSourceMembers));
    SampleSourcePcmData extraData = (SampleSourcePcmData)malloc(sizeof(SampleSourcePcmDataMembers));

    sampleSource->sampleSourceType = SAMPLE_SOURCE_TYPE_PCM;
    sampleSource->openedAs = SAMPLE_SOURCE_OPEN_NOT_OPENED;
    sampleSource->sourceName = newCharString();
    charStringCopy(sampleSource->sourceName, sampleSourceName);
    sampleSource->numSamplesProcessed = 0;

    sampleSource->openSampleSource = openSampleSourcePcm;
    sampleSource->readSampleBlock = readBlockFromPcmFile;
    sampleSource->writeSampleBlock = writeBlockToPcmFile;
    sampleSource->closeSampleSource = _closeSampleSourcePcm;
    sampleSource->freeSampleSourceData = freeSampleSourceDataPcm;

    extraData->isStream = false;
    extraData->isLittleEndian = true;
    extraData->fileHandle = NULL;
    // Assume default values for these items. However, if an incoming SampleBuffer
    // has different values for the channel count or blocksize, then we will reassign
    // based on those values.
    extraData->dataBufferNumItems = getNumChannels() * getBlocksize();
    extraData->pcmSampleBuffer = newPcmSampleBuffer(getNumChannels(), getBlocksize(), getBitDepth());

    extraData->numChannels = getNumChannels();
    extraData->sampleRate = getSampleRate();
    extraData->bitDepth = getBitDepth();
    sampleSource->extraData = extraData;

    return sampleSource;
}
