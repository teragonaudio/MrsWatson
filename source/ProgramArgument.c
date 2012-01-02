//
//  ProgramArgument.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ProgramArgument.h"

ProgramArgument newProgramArgument(const char* name, const char* help, bool isShort) {
  ProgramArgument programArgument = malloc(sizeof(ProgramArgumentMembers));

  programArgument->name = newCharStringShort();
  strncpy(programArgument->name, name, STRING_LENGTH);
  programArgument->help = newCharStringLong();
  strncpy(programArgument->help, help, STRING_LENGTH_LONG);
  programArgument->isShort = false;

  return programArgument;
}

ProgramArguments newProgramArguments(void) {
}