//
//  ProgramOption.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#import "CharString.h"
#import "Types.h"

#ifndef MrsWatson_ProgramOption_h
#define MrsWatson_ProgramOption_h

typedef struct {
  CharString name;
  CharString help;
  bool isShort;
  bool requiresArgument;
} ProgramOptionMembers;

typedef ProgramOptionMembers* ProgramOption;

#endif
