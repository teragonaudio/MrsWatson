//
//  ProgramOption.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "CharString.h"
#include "Types.h"

#ifndef MrsWatson_ProgramOption_h
#define MrsWatson_ProgramOption_h

typedef struct {
  int index;
  CharString name;
  CharString help;
  bool isShort;
  bool requiresArgument;
  CharString argument;
} ProgramOptionMembers;

typedef enum {
  OPTION_INPUT_SOURCE,
  OPTION_HELP,
  OPTION_VERSION,
  OPTION_VERBOSE,
  OPTION_QUIET,
  NUM_OPTIONS
} ProgramOptionIndex;

typedef ProgramOptionMembers* ProgramOption;
typedef ProgramOption* ProgramOptions;

ProgramOptions newProgramOptions(void);
ProgramOption findProgramOption(ProgramOptions programOptions, const char* optionString);
bool fillOptionArgument(ProgramOption programOption, int* currentArgc, int argc, char** argv);
void printProgramOptions(ProgramOptions programOptions);
void freeProgramOptions(ProgramOptions programOptions);

#endif
