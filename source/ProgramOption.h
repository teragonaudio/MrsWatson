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

typedef enum {
  ARGUMENT_TYPE_NONE,
  ARGUMENT_TYPE_OPTIONAL,
  ARGUMENT_TYPE_REQUIRED
} ProgramOptionArgumentType;

typedef struct {
  int index;
  CharString name;
  CharString help;
  bool hasShortForm;

  ProgramOptionArgumentType argumentType;
  CharString stringArgument;
  bool enabled;
} ProgramOptionMembers;

typedef enum {
  OPTION_INPUT_SOURCE,
  OPTION_HELP,
  OPTION_VERSION,
  OPTION_COLOR_LOGGING,
  OPTION_VERBOSE,
  OPTION_QUIET,
  NUM_OPTIONS
} ProgramOptionIndex;

typedef ProgramOptionMembers* ProgramOption;
typedef ProgramOption* ProgramOptions;

ProgramOptions newProgramOptions(void);
bool parseCommandLine(ProgramOptions programOptions, int argc, char** argv);
void printProgramOptions(ProgramOptions programOptions);
void freeProgramOptions(ProgramOptions programOptions);

#endif
