//
//  ProgramOptions.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "ProgramOptions.h"

ProgramOptions newProgramOptions(void) {
  ProgramOptions programOptions = malloc(sizeof(ProgramOptionsMembers));
  
  programOptions->quiet = false;
  programOptions->verbose = false;
  
  return programOptions;
}

bool parseCommandLine(ProgramOptions programOptions, int argc, char *argv[]) {
  return true;
}
