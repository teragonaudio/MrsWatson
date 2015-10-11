//
// SampleSource.c - MrsWatson
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

#include "base/File.h"
#include "io/SampleSource.h"
#include "logging/EventLogger.h"

void sampleSourcePrintSupportedTypes(void)
{
    logInfo("Supported audio file types:");
    // We can theoretically support more formats, pretty much anything audiofile supports
    // would work here. However, most of those file types are rather uncommon, and require
    // special setup when writing, so we only choose the most common ones.
#if USE_AUDIOFILE
    logInfo("- AIFF (via libaudiofile)");
#endif
#if USE_FLAC
    logInfo("- FLAC (via libaudiofile)");
#endif

    // Always supported
    logInfo("- PCM");

#if USE_AUDIOFILE
    logInfo("- WAV (via libaudiofile)");
#else
    logInfo("- WAV (internal)");
#endif
}

static SampleSourceType _sampleSourceGuess(const CharString sampleSourceName)
{
    File sourceFile = NULL;
    CharString sourceFileExtension = NULL;
    SampleSourceType result = SAMPLE_SOURCE_TYPE_PCM;

    if (sampleSourceName == NULL || charStringIsEmpty(sampleSourceName)) {
        result = SAMPLE_SOURCE_TYPE_SILENCE;
    } else {
        // Look for stdin/stdout
        if (strlen(sampleSourceName->data) == 1 && sampleSourceName->data[0] == '-') {
            result = SAMPLE_SOURCE_TYPE_PCM;
        } else {
            sourceFile = newFileWithPath(sampleSourceName);
            sourceFileExtension = fileGetExtension(sourceFile);
            freeFile(sourceFile);

            // If there is no file extension, then automatically assume raw PCM data. Deal with it!
            if (charStringIsEmpty(sourceFileExtension)) {
                result = SAMPLE_SOURCE_TYPE_PCM;
            }
            // Possible file extensions for raw PCM data
            else if (charStringIsEqualToCString(sourceFileExtension, "pcm", true) ||
                     charStringIsEqualToCString(sourceFileExtension, "raw", true) ||
                     charStringIsEqualToCString(sourceFileExtension, "dat", true)) {
                result = SAMPLE_SOURCE_TYPE_PCM;
            }

#if USE_AUDIOFILE
            else if (charStringIsEqualToCString(sourceFileExtension, "aif", true) ||
                     charStringIsEqualToCString(sourceFileExtension, "aiff", true)) {
                result = SAMPLE_SOURCE_TYPE_AIFF;
            }

#endif

#if USE_FLAC
            else if (charStringIsEqualToCString(sourceFileExtension, "flac", true)) {
                result = SAMPLE_SOURCE_TYPE_FLAC;
            }

#endif

            else if (charStringIsEqualToCString(sourceFileExtension, "wav", true) ||
                     charStringIsEqualToCString(sourceFileExtension, "wave", true)) {
                result = SAMPLE_SOURCE_TYPE_WAVE;
            } else {
                logCritical("Sample source '%s' does not match any supported type", sampleSourceName->data);
                result = SAMPLE_SOURCE_TYPE_INVALID;
            }
        }
    }

    freeCharString(sourceFileExtension);
    return result;
}

extern SampleSource _newSampleSourceAudiofile(const CharString sampleSourceName,
        const SampleSourceType sampleSourceType);
extern SampleSource _newSampleSourcePcm(const CharString sampleSourceName);
extern SampleSource _newSampleSourceSilence();
extern SampleSource _newSampleSourceWave(const CharString sampleSourceName);

SampleSource sampleSourceFactory(const CharString sampleSourceName)
{
    SampleSourceType sampleSourceType = _sampleSourceGuess(sampleSourceName);

    switch (sampleSourceType) {
    case SAMPLE_SOURCE_TYPE_SILENCE:
        return _newSampleSourceSilence();

    case SAMPLE_SOURCE_TYPE_PCM:
        return _newSampleSourcePcm(sampleSourceName);

#if USE_AUDIOFILE

    case SAMPLE_SOURCE_TYPE_AIFF:
        return _newSampleSourceAudiofile(sampleSourceName, sampleSourceType);
#endif

#if USE_FLAC

    case SAMPLE_SOURCE_TYPE_FLAC:
        return _newSampleSourceAudiofile(sampleSourceName, sampleSourceType);
#endif

#if USE_AUDIOFILE

    case SAMPLE_SOURCE_TYPE_WAVE:
        return _newSampleSourceAudiofile(sampleSourceName, sampleSourceType);
#else

    case SAMPLE_SOURCE_TYPE_WAVE:
        return _newSampleSourceWave(sampleSourceName);
#endif

    default:
        return NULL;
    }
}

void freeSampleSource(SampleSource self)
{
    if (self != NULL) {
        self->freeSampleSourceData(self->extraData);
        freeCharString(self->sourceName);
        free(self);
    }
}
