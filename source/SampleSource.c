//
//  SampleSource.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SampleSource.h"
#include "SampleSourcePcmFile.h"
#include "StringUtilities.h"
#include "EventLogger.h"
#include "SampleSourceSilence.h"

SampleSourceType guessSampleSourceType(const CharString sampleSourceTypeString) {
  if(!isCharStringEmpty(sampleSourceTypeString)) {
    // Look for stdin/stdout
    if(strlen(sampleSourceTypeString->data) == 1 && sampleSourceTypeString->data[0] == '-') {
      return SAMPLE_SOURCE_TYPE_PCM_STREAM;
    }
    else {
      const char* fileExtension = getFileExtension(sampleSourceTypeString->data);
      // If there is no file extension, then automatically assume raw PCM data. Deal with it!
      if(fileExtension == NULL) {
        return SAMPLE_SOURCE_TYPE_PCM_FILE;
      }
      // Possible file extensions for raw PCM data
      else if(!strcasecmp(fileExtension, "pcm") || !strcasecmp(fileExtension, "raw") || !strcasecmp(fileExtension, "dat")) {
        return SAMPLE_SOURCE_TYPE_PCM_FILE;
      }
      else {
        logCritical("Sample source '%s' does not match any supported type", sampleSourceTypeString->data);
        return SAMPLE_SOURCE_TYPE_INVALID;
      }
    }
  }
  else {
    logInternalError("Input source type was null");
    return SAMPLE_SOURCE_TYPE_INVALID;
  }
}

SampleSource newSampleSource(SampleSourceType sampleSourceType, const CharString sampleSourceName) {
  switch(sampleSourceType) {
    case SAMPLE_SOURCE_TYPE_PCM_FILE:
      return newSampleSourcePcmFile(sampleSourceName);
    case SAMPLE_SOURCE_TYPE_PCM_STREAM:
      // TODO: Currently unsupported
      return NULL;
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
