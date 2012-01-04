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
  boolean hasShortForm;

  ProgramOptionArgumentType argumentType;
  CharString argument;
  boolean enabled;
} ProgramOptionMembers;

typedef enum {
  OPTION_BLOCKSIZE,
  OPTION_CHANNELS,
  OPTION_COLOR_LOGGING,
  OPTION_DISPLAY_INFO,
  OPTION_HELP,
  OPTION_INPUT_SOURCE,
  OPTION_OUTPUT_SOURCE,
  OPTION_PCM_FILE_NUM_CHANNELS,
  OPTION_PCM_FILE_SAMPLERATE,
  OPTION_PLUGIN,
  OPTION_QUIET,
  OPTION_SAMPLERATE,
  OPTION_VERBOSE,
  OPTION_VERSION,
  NUM_OPTIONS
} ProgramOptionIndex;

typedef ProgramOptionMembers* ProgramOption;
typedef ProgramOption* ProgramOptions;

ProgramOptions newProgramOptions(void);
boolean parseCommandLine(ProgramOptions programOptions, int argc, char** argv);
void printProgramOptions(ProgramOptions programOptions);
void freeProgramOptions(ProgramOptions programOptions);

#endif
