//
// SampleSourceAudiofile.c - MrsWatson
// Created by Nik Reiman on 1/22/12.
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

#if USE_AUDIOFILE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "audio/AudioSettings.h"
#include "io/SampleSourceAudiofile.h"
#include "io/SampleSourcePcm.h"
#include "logging/EventLogger.h"

static boolByte _openSampleSourceAudiofile(void *sampleSourcePtr, const SampleSourceOpenAs openAs)
{
    SampleSource sampleSource = (SampleSource)sampleSourcePtr;
    SampleSourceAudiofileData extraData = sampleSource->extraData;

    if (openAs == SAMPLE_SOURCE_OPEN_READ) {
        extraData->fileHandle = afOpenFile(sampleSource->sourceName->data, "r", NULL);

        if (extraData->fileHandle != NULL) {
            setNumChannels(afGetVirtualChannels(extraData->fileHandle, AF_DEFAULT_TRACK));
            setSampleRate((float)afGetRate(extraData->fileHandle, AF_DEFAULT_TRACK));
        }
    } else if (openAs == SAMPLE_SOURCE_OPEN_WRITE) {
        AFfilesetup outfileSetup = afNewFileSetup();
        afInitFileFormat(outfileSetup, AF_FILE_WAVE);
        afInitByteOrder(outfileSetup, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);
        afInitChannels(outfileSetup, AF_DEFAULT_TRACK, getNumChannels());
        afInitRate(outfileSetup, AF_DEFAULT_TRACK, getSampleRate());
        afInitSampleFormat(outfileSetup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, DEFAULT_BITRATE);
        extraData->fileHandle = afOpenFile(sampleSource->sourceName->data, "w", outfileSetup);
    } else {
        logInternalError("Invalid type for openAs in WAVE file");
        return false;
    }

    if (extraData->fileHandle == NULL) {
        logError("WAVE file '%s' could not be opened for %s",
                sampleSource->sourceName->data, openAs == SAMPLE_SOURCE_OPEN_READ ? "reading" : "writing");
        return false;
    }

    sampleSource->openedAs = openAs;
    return true;
}

boolByte _readBlockFromAudiofile(void *sampleSourcePtr, SampleBuffer sampleBuffer)
{
    SampleSource sampleSource = (SampleSource)sampleSourcePtr;
    SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)(sampleSource->extraData);
    const size_t bytesToRead = sizeof(float) * getNumChannels() * getBlocksize();
    AFframecount numFramesRead;

    int currentInterlacedSample = 0;
    int currentDeinterlacedSample = 0;
    int currentChannel;

    if (extraData->interlacedBuffer == NULL) {
        extraData->interlacedBuffer = (float *)malloc(bytesToRead);
    }

    memset(extraData->interlacedBuffer, 0, bytesToRead);

    numFramesRead = afReadFrames(extraData->fileHandle, AF_DEFAULT_TRACK,
            extraData->interlacedBuffer, (int)getBlocksize());

    // Loop over the number of frames wanted, not the number we actually got. This means that the last block will
    // be partial, but then we write empty data to the end, since the interlaced buffer gets cleared above.
    while (currentInterlacedSample < getBlocksize() * getNumChannels()) {
        for (currentChannel = 0; currentChannel < sampleBuffer->numChannels; currentChannel++) {
            sampleBuffer->samples[currentChannel][currentDeinterlacedSample] = extraData->interlacedBuffer[currentInterlacedSample++];
        }

        currentDeinterlacedSample++;
    }

    // Set the blocksize of the sample buffer to be the number of frames read
    sampleBuffer->blocksize = (unsigned long)numFramesRead;
    sampleSource->numSamplesProcessed += numFramesRead;

    if (numFramesRead == 0) {
        logDebug("End of audio file reached");
        return false;
    } else if (numFramesRead < 0) {
        logError("Error reading audio file");
        return false;
    } else {
        return true;
    }
}

boolByte _writeBlockToAudiofile(void *sampleSourcePtr, const SampleBuffer sampleBuffer)
{
    SampleSource sampleSource = (SampleSource)sampleSourcePtr;
    SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)(sampleSource->extraData);
    const AFframecount numSamplesToWrite = sampleBuffer->blocksize * sampleBuffer->numChannels;
    AFframecount numFramesWritten = 0;

    if (extraData->pcmBuffer == NULL) {
        extraData->pcmBuffer = (short *)malloc(sizeof(short) * getNumChannels() * getBlocksize());
    }

    memset(extraData->pcmBuffer, 0, sizeof(short) * getNumChannels() * getBlocksize());
    // TODO: flip endian property is probably wrong!!
    sampleBufferGetPcmSamples(sampleBuffer, extraData->pcmBuffer, false);

    numFramesWritten = afWriteFrames(extraData->fileHandle, AF_DEFAULT_TRACK,
            extraData->pcmBuffer, (int)getBlocksize());
    sampleSource->numSamplesProcessed += getBlocksize() * getNumChannels();

    if (numFramesWritten == -1) {
        logWarn("audiofile encountered an error when writing to file");
        return false;
    } else if (numFramesWritten == numSamplesToWrite) {
        return true;
    } else {
        logWarn("Short write occurred while writing samples");
        return false;
    }
}

void _closeSampleSourceAudiofile(void *sampleSourcePtr)
{
    SampleSource sampleSource = (SampleSource)sampleSourcePtr;
    SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)sampleSource->extraData;

    if (extraData->fileHandle != NULL) {
        afCloseFile(extraData->fileHandle);
    }
}

void _freeSampleSourceDataAudiofile(void *sampleSourceDataPtr)
{
    SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)sampleSourceDataPtr;

    if (extraData->interlacedBuffer != NULL) {
        free(extraData->interlacedBuffer);
    }

    if (extraData->pcmBuffer != NULL) {
        free(extraData->pcmBuffer);
    }

    free(extraData);
}

SampleSource _newSampleSourceAudiofile(const CharString sampleSourceName,
        const SampleSourceType sampleSourceType)
{
    SampleSource sampleSource = (SampleSource)malloc(sizeof(SampleSourceMembers));
    SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)malloc(sizeof(SampleSourceAudiofileDataMembers));

    sampleSource->sampleSourceType = sampleSourceType;
    sampleSource->openedAs = SAMPLE_SOURCE_OPEN_NOT_OPENED;
    sampleSource->sourceName = newCharString();
    charStringCopy(sampleSource->sourceName, sampleSourceName);
    sampleSource->numSamplesProcessed = 0;

    sampleSource->openSampleSource = _openSampleSourceAudiofile;
    sampleSource->readSampleBlock = _readBlockFromAudiofile;
    sampleSource->writeSampleBlock = _writeBlockToAudiofile;
    sampleSource->closeSampleSource = _closeSampleSourceAudiofile;
    sampleSource->freeSampleSourceData = _freeSampleSourceDataAudiofile;

    extraData->fileHandle = NULL;
    extraData->interlacedBuffer = NULL;
    extraData->pcmBuffer = NULL;

    sampleSource->extraData = extraData;
    return sampleSource;
}

#endif
