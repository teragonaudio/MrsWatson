//
//  ProgramArgument.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#import "CharString.h"
#import "Types.h"

#ifndef MrsWatson_ProgramArgument_h
#define MrsWatson_ProgramArgument_h

typedef struct {
  CharString name;
  CharString help;
  bool isShort;
  bool requiresArgument;
} ProgramArgumentMembers;

typedef ProgramArgumentMembers* ProgramArgument;
typedef ProgramArgument* ProgramArguments;

ProgramArguments newProgramArguments(void);

#endif
