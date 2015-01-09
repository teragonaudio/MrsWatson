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
#include <stdlib.h>

#include "audio/AudioSettings.h"
#include "audio/PcmSampleBuffer.h"
#include "io/SampleSourceAudiofile.h"
#include "io/SampleSourcePcm.h"
#include "logging/EventLogger.h"

static boolByte _openSampleSourceAudiofile(void *selfPtr, const SampleSourceOpenAs openAs)
{
    SampleSource self = (SampleSource) selfPtr;
    SampleSourceAudiofileData extraData = self->extraData;

    if (openAs == SAMPLE_SOURCE_OPEN_READ) {
        extraData->fileHandle = afOpenFile(self->sourceName->data, "r", NULL);

        if (extraData->fileHandle != NULL) {
            setNumChannels((const ChannelCount)afGetVirtualChannels(extraData->fileHandle, AF_DEFAULT_TRACK));
            setSampleRate((const SampleRate)afGetRate(extraData->fileHandle, AF_DEFAULT_TRACK));
            int sampleFormat = 0;
            int bitDepth = 0;
            afGetSampleFormat(extraData->fileHandle, AF_DEFAULT_TRACK, &sampleFormat, &bitDepth);
            setBitDepth((BitDepth) bitDepth);
            extraData->pcmSampleBuffer = newPcmSampleBuffer(getNumChannels(), getBlocksize(), getBitDepth());
            logDebug("Opened audiofile %d-bit, %s-endian for reading",
                     extraData->pcmSampleBuffer->bitDepth,
                     extraData->pcmSampleBuffer->littleEndian ? "little" : "big");
        }
    } else if (openAs == SAMPLE_SOURCE_OPEN_WRITE) {
        int byteOrder = AF_BYTEORDER_LITTLEENDIAN;

        int sampleFormat;
        switch (getBitDepth()) {
            case kBitDepth8Bit:
               sampleFormat = AF_SAMPFMT_UNSIGNED;
                break;
            case kBitDepth32Bit:
                sampleFormat = AF_SAMPFMT_FLOAT;
                break;
            default:
                sampleFormat = AF_SAMPFMT_TWOSCOMP;
                break;
        }

        int outfileFormat;
        switch (self->sampleSourceType) {
            case SAMPLE_SOURCE_TYPE_AIFF:
                // AIFF is the only file format we support which is big-endian. That is,
                // even on big-endian platforms (which are untested), raw PCM should still
                // write little-endian data.
                byteOrder = AF_BYTEORDER_BIGENDIAN;
                outfileFormat = AF_FILE_AIFF;
                break;
            case SAMPLE_SOURCE_TYPE_WAVE:
                outfileFormat = AF_FILE_WAVE;
                break;
            case SAMPLE_SOURCE_TYPE_FLAC:
                outfileFormat = AF_FILE_FLAC;
                break;
            default:
                logInternalError("Unsupported audiofile type %d", self->sampleSourceType);
                return false;
        }

        AFfilesetup outfileSetup = afNewFileSetup();
        afInitFileFormat(outfileSetup, outfileFormat);
        afInitByteOrder(outfileSetup, AF_DEFAULT_TRACK, byteOrder);
        afInitChannels(outfileSetup, AF_DEFAULT_TRACK, getNumChannels());
        afInitRate(outfileSetup, AF_DEFAULT_TRACK, getSampleRate());
        afInitSampleFormat(outfileSetup, AF_DEFAULT_TRACK, sampleFormat, getBitDepth());
        extraData->fileHandle = afOpenFile(self->sourceName->data, "w", outfileSetup);
        extraData->pcmSampleBuffer = newPcmSampleBuffer(getNumChannels(), getBlocksize(), getBitDepth());
        extraData->pcmSampleBuffer->littleEndian = (boolByte)(byteOrder == AF_BYTEORDER_LITTLEENDIAN);
        logDebug("Opened audiofile %d-bit, %s-endian for writing",
                 extraData->pcmSampleBuffer->bitDepth,
                 extraData->pcmSampleBuffer->littleEndian ? "little" : "big");
        afFreeFileSetup(outfileSetup);
    } else {
        logInternalError("Invalid type for openAs in audiofile source");
        return false;
    }

    if (extraData->fileHandle == NULL) {
        logError("File '%s' could not be opened for %s",
                 self->sourceName->data,
                 openAs == SAMPLE_SOURCE_OPEN_READ ? "reading" : "writing");
        return false;
    }

    self->openedAs = openAs;
    return true;
}

boolByte _readBlockFromAudiofile(void *selfPtr, SampleBuffer sampleBuffer)
{
    SampleSource self = (SampleSource)selfPtr;
    SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)(self->extraData);
    const SampleBuffer superSampleBuffer = extraData->pcmSampleBuffer->getSampleBuffer(extraData->pcmSampleBuffer);
    AFframecount numFramesRead = 0;

    // If the blocksize has changed, then regenerate our PCM sample buffer to
    // make room for it.
    if (superSampleBuffer->blocksize != sampleBuffer->blocksize ||
            superSampleBuffer->numChannels != sampleBuffer->numChannels) {
        freePcmSampleBuffer(extraData->pcmSampleBuffer);
        extraData->pcmSampleBuffer = newPcmSampleBuffer(sampleBuffer->numChannels, sampleBuffer->blocksize, getBitDepth());
    }

    numFramesRead = afReadFrames(extraData->fileHandle,
                                 AF_DEFAULT_TRACK,
                                 extraData->pcmSampleBuffer->pcmSamples,
                                 (int)getBlocksize());
    extraData->pcmSampleBuffer->setSamples(extraData->pcmSampleBuffer);

    sampleBufferCopyAndMapChannels(sampleBuffer, extraData->pcmSampleBuffer->getSampleBuffer(extraData->pcmSampleBuffer));

    // Set the blocksize of the sample buffer to be the number of frames read
    sampleBuffer->blocksize = (SampleCount)numFramesRead;
    self->numSamplesProcessed += numFramesRead;

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

boolByte _writeBlockToAudiofile(void *selfPtr, const SampleBuffer sampleBuffer)
{
    SampleSource self = (SampleSource) selfPtr;
    SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)(self->extraData);
    const AFframecount numSamplesToWrite = sampleBuffer->blocksize;
    AFframecount numFramesWritten = 0;

    extraData->pcmSampleBuffer->setSampleBuffer(extraData->pcmSampleBuffer, sampleBuffer);
    numFramesWritten = afWriteFrames(extraData->fileHandle,
                                     AF_DEFAULT_TRACK,
                                     extraData->pcmSampleBuffer->pcmSamples,
                                     (int)getBlocksize());
    self->numSamplesProcessed += getBlocksize() * getNumChannels();

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

void _closeSampleSourceAudiofile(void *selfPtr)
{
    SampleSource self = (SampleSource) selfPtr;
    SampleSourceAudiofileData extraData = (SampleSourceAudiofileData) self->extraData;

    if (extraData->fileHandle != NULL) {
        afCloseFile(extraData->fileHandle);
    }
}

void _freeSampleSourceDataAudiofile(void *extraDataPtr)
{
    SampleSourceAudiofileData extraData = (SampleSourceAudiofileData) extraDataPtr;
    if (extraData->pcmSampleBuffer != NULL) {
        freePcmSampleBuffer(extraData->pcmSampleBuffer);
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
    extraData->pcmSampleBuffer = NULL;

    sampleSource->extraData = extraData;
    return sampleSource;
}

#endif
