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

#include "base/FileUtilities.h"
#include "base/PlatformUtilities.h"
#include "base/StringUtilities.h"
#include "io/SampleSourceAiff.h"
#include "io/SampleSourceFlac.h"
#include "io/SampleSource.h"
#include "io/SampleSourcePcm.h"
#include "io/SampleSourceSilence.h"
#include "io/SampleSourceWave.h"
#include "logging/EventLogger.h"

void sampleSourcePrintSupportedTypes(void) {
  logInfo("Supported audio file types:");
  // We can theoretically support more formats, pretty much anything audiofile supports
  // would work here. However, most of those file types are rather uncommon, and require
  // special setup when writing, so we only choose the most common ones.
#if HAVE_LIBAUDIOFILE
  logInfo("- AIFF (via libaudiofile)");
#else
  logInfo("- AIFF (internal, experimental)");
#endif
#if HAVE_LIBFLAC
  logInfo("- FLAC");
#endif
#if HAVE_LIBLAME
  logInfo("- MP3");
#endif
#if HAVE_LIBVORBIS
  logInfo("- OGG");
#endif
  // Always supported
  logInfo("- PCM");
#if HAVE_LIBAUDIOFILE
  logInfo("- WAV (via libaudiofile)");
#else
  logInfo("- WAV (internal)");
#endif
}

SampleSourceType sampleSourceGuess(const CharString sampleSourceTypeString) {
  if(!charStringIsEmpty(sampleSourceTypeString)) {
    // Look for stdin/stdout
    if(strlen(sampleSourceTypeString->data) == 1 && sampleSourceTypeString->data[0] == '-') {
      return SAMPLE_SOURCE_TYPE_PCM;
    }
    else {
      const char* fileExtension = getFileExtension(sampleSourceTypeString->data);
      // If there is no file extension, then automatically assume raw PCM data. Deal with it!
      if(fileExtension == NULL) {
        return SAMPLE_SOURCE_TYPE_PCM;
      }
      // Possible file extensions for raw PCM data
      else if(!strcasecmp(fileExtension, "pcm") || !strcasecmp(fileExtension, "raw") || !strcasecmp(fileExtension, "dat")) {
        return SAMPLE_SOURCE_TYPE_PCM;
      }
      else if(!strcasecmp(fileExtension, "aif") || !strcasecmp(fileExtension, "aiff")) {
        return SAMPLE_SOURCE_TYPE_AIFF;
      }
#if HAVE_LIBFLAC
      else if(!strcasecmp(fileExtension, "flac")) {
        return SAMPLE_SOURCE_TYPE_FLAC;
      }
#endif
#if HAVE_LIBLAME
      else if(!strcasecmp(fileExtension, "mp3")) {
        return SAMPLE_SOURCE_TYPE_MP3;
      }
#endif
#if HAVE_LIBVORBIS
      else if(!strcasecmp(fileExtension, "ogg")) {
        return SAMPLE_SOURCE_TYPE_OGG;
      }
#endif
      else if(!strcasecmp(fileExtension, "wav") || !strcasecmp(fileExtension, "wave")) {
        return SAMPLE_SOURCE_TYPE_WAVE;
      }
      else {
        logCritical("Sample source '%s' does not match any supported type", sampleSourceTypeString->data);
        return SAMPLE_SOURCE_TYPE_INVALID;
      }
    }
  }
  else {
    return SAMPLE_SOURCE_TYPE_INVALID;
  }
}

SampleSource newSampleSource(SampleSourceType sampleSourceType, const CharString sampleSourceName) {
  switch(sampleSourceType) {
    case SAMPLE_SOURCE_TYPE_SILENCE:
      return newSampleSourceSilence();
    case SAMPLE_SOURCE_TYPE_PCM:
      return newSampleSourcePcm(sampleSourceName);
    case SAMPLE_SOURCE_TYPE_AIFF:
      return newSampleSourceAiff(sampleSourceName);
#if HAVE_LIBFLAC
    case SAMPLE_SOURCE_TYPE_FLAC:
      return newSampleSourceFlac(sampleSourceName);
#endif
#if HAVE_LIBLAME
    case SAMPLE_SOURCE_TYPE_MP3:
      return newSampleSourceMp3(sampleSourceName);
#endif
#if HAVE_LIBVORBIS
    case SAMPLE_SOURCE_TYPE_OGG:
      return newSampleSourceOgg(sampleSourceName);
#endif
    case SAMPLE_SOURCE_TYPE_WAVE:
      return newSampleSourceWave(sampleSourceName);
    default:
      return NULL;
  }
}

void freeSampleSource(SampleSource sampleSource) {
  sampleSource->freeSampleSourceData(sampleSource->extraData);
  freeCharString(sampleSource->sourceName);
  free(sampleSource);
}
