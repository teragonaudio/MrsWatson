//
//  InputSourcePcmFile.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include "InputSource.h"

#ifndef MrsWatson_InputSourcePcmFile_h
#define MrsWatson_InputSourcePcmFile_h

typedef struct {
  FILE* fileHandle;
  size_t dataBufferNumItems;
  short* interlacedPcmDataBuffer;
  Sample* interlacedSampleBuffer;
} InputSourcePcmFileDataMembers;

typedef InputSourcePcmFileDataMembers* InputSourcePcmFileData;

InputSource newInputSourcePcmFile(void);

#endif
