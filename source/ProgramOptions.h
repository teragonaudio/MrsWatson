//
//  ProgramOptions.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "Types.h"

#ifndef MrsWatson_ProgramOptions_h
#define MrsWatson_ProgramOptions_h

typedef struct {
  bool verbose;
  bool quiet;
} ProgramOptionsMembers;

typedef ProgramOptionsMembers* ProgramOptions;

ProgramOptions newProgramOptions(void);
bool parseCommandLine(ProgramOptions programOptions, int argc, char** argv);

#endif
