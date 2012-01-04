//
//  InputSource.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "InputSource.h"
#include "InputSourcePcmFile.h"
#include "InputSourcePcmStream.h"
#import "StringUtilities.h"

InputSourceType guessInputSourceType(CharString inputSourceTypeString) {
  if(inputSourceTypeString != NULL) {
    // Look for stdin/stdout
    if(strlen(inputSourceTypeString->data) == 1 && inputSourceTypeString->data[0] == '-') {
      return INPUT_SOURCE_TYPE_PCM_STREAM;
    }
    else {
      const char* fileExtension = getFileExtension(inputSourceTypeString->data);
      // If there is no file extension, then automatically assume raw PCM data. Deal with it!
      if(fileExtension == NULL) {
        return INPUT_SOURCE_TYPE_PCM_FILE;
      }
      // Possible file extensions for raw PCM data
      else if(!strcasecmp(fileExtension, "pcm") || !strcasecmp(fileExtension, "raw") || !strcasecmp(fileExtension, "dat")) {
        return INPUT_SOURCE_TYPE_PCM_FILE;
      }
    }
  }

  // If we reach this point, then it is probably not a supported input source type
  return INPUT_SOURCE_TYPE_INVALID;
}

InputSource newInputSource(InputSourceType inputSourceType, const CharString inputSourceName) {
  switch(inputSourceType) {
    case INPUT_SOURCE_TYPE_PCM_FILE:
      return newInputSourcePcmFile(inputSourceName);
    case INPUT_SOURCE_TYPE_PCM_STREAM:
      return newInputSourcePcmStream();
    default:
      return NULL;
  }
}

void freeInputSource(InputSource inputSource) {
  inputSource->freeInputSourceData(inputSource->extraData);
  freeCharString(inputSource->inputSourceName);
  free(inputSource);
}
