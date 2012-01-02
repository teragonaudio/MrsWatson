//
//  InputSource.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#import "Types.h"
#import "SampleBuffer.h"
#import "CharString.h"

#ifndef MrsWatson_InputFile_h
#define MrsWatson_InputFile_h

typedef void(*ReadBlockFuncPtr)(SampleBuffer);

typedef struct {
  CharString inputName;
  ReadBlockFuncPtr readBlockFuncPtr;
} InputFileMembers;

typedef InputFileMembers* InputFile;

#endif
