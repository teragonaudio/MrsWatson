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
#include "SampleSource.h"
#include "SampleSourcePcm.h"
#include "StringUtilities.h"
#include "EventLogger.h"
#include "SampleSourceSilence.h"

void printSupportedSourceTypes(void) {
  printf("- Raw PCM\n");
  logUnsupportedFeature("We could use support for more!");
}

SampleSourceType guessSampleSourceType(const CharString sampleSourceTypeString) {
  if(!isCharStringEmpty(sampleSourceTypeString)) {
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
      else {
        logCritical("Sample source '%s' does not match any supported type", sampleSourceTypeString->data);
        return SAMPLE_SOURCE_TYPE_INVALID;
      }
    }
  }
  else {
    logInternalError("Sample source type was null");
    return SAMPLE_SOURCE_TYPE_INVALID;
  }
}

SampleSource newSampleSource(SampleSourceType sampleSourceType, const CharString sampleSourceName) {
  switch(sampleSourceType) {
    case SAMPLE_SOURCE_TYPE_PCM:
      return newSampleSourcePcm(sampleSourceName);
    case SAMPLE_SOURCE_TYPE_SILENCE:
      return newSampleSourceSilence();
    default:
      return NULL;
  }
}

void freeSampleSource(SampleSource sampleSource) {
  sampleSource->freeSampleSourceData(sampleSource->extraData);
  freeCharString(sampleSource->sourceName);
  free(sampleSource);
}
