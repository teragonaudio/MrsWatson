//
//  ProgramOption.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ProgramOption.h"

static void _addNewProgramOption(const ProgramOptions programOptions, const int index,
  const char* name, const char* help, bool isShort, bool requiresArgument) {
  ProgramOption programOption = malloc(sizeof(ProgramOptionMembers));

  programOption->index = index;
  programOption->name = newCharStringShort();
  strncpy(programOption->name, name, STRING_LENGTH);
  programOption->help = newCharStringLong();
  strncpy(programOption->help, help, STRING_LENGTH_LONG);
  programOption->isShort = isShort;
  programOption->requiresArgument = requiresArgument;

  programOptions[index] = programOption;
}

ProgramOption* newProgramOptions(void) {
  ProgramOptions programOptions = malloc(sizeof(ProgramOption) * NUM_OPTIONS);

  _addNewProgramOption(programOptions, OPTION_HELP, "help", "Print help", true, false);
  _addNewProgramOption(programOptions, OPTION_VERSION, "version", "Print version and copyright information", false, false);
  _addNewProgramOption(programOptions, OPTION_VERBOSE, "verbose", "Verbose logging", true, false);
  _addNewProgramOption(programOptions, OPTION_QUIET, "quiet", "Only log critical errors", true, false);

  return programOptions;
}

ProgramOption findProgramOption(ProgramOptions programOptions, const char* optionString) {
  // Extra safety check
  if(optionString != NULL) {
    // Look for short options, and return immediately if one is found
    if(strlen(optionString) == 2 && optionString[0] == '-') {
      for(int i = 0; i < NUM_OPTIONS; i++) {
        ProgramOption potentialMatchOption = programOptions[i];
        if(potentialMatchOption->isShort && potentialMatchOption->name[0] == optionString[1]) {
          return potentialMatchOption;
        }
      }
    }

    // Verify that the argument is a long argument (ie, --argument)
    if(strlen(optionString) > 2 && optionString[0] == '-' && optionString[1] == '-') {
      ProgramOption optionMatch = NULL;
      CharString optionStringWithoutDashes = newCharStringShort();
      strncpy(optionStringWithoutDashes, optionString + 2, strlen(optionString) - 2);
      for(int i = 0; i < NUM_OPTIONS; i++) {
        ProgramOption potentialMatchOption = programOptions[i];
        if(!strncmp(potentialMatchOption->name, optionStringWithoutDashes, STRING_LENGTH_SHORT)) {
          optionMatch = potentialMatchOption;
          break;
        }
      }
      free(optionStringWithoutDashes);
      return optionMatch;
    }
  }

  // If no option was found, then return null
  return NULL;
}

void printProgramOptions(ProgramOptions programOptions) {
  for(int i = 0; i < NUM_OPTIONS; i++) {
    printf("  ");
    ProgramOption programOption = programOptions[i];

    if(programOption->isShort) {
      printf("-%c, ", programOption->name[0]);
    }

    // All arguments have a long form
    printf("--%s", programOption->name);

    if(programOption->requiresArgument) {
      printf("=(argument)");
    }

    // Newline and indentation before help
    CharString wrappedHelpString = newCharStringLong();
    wrapCharStringForTerminal(programOption->help, wrappedHelpString, 4);
    printf("\n    %s\n", programOption->help);
    free(wrappedHelpString);
  }
}

static void _freeProgramOption(ProgramOption programOption) {
  free(programOption->name);
  free(programOption->help);
  free(programOption);
}

void freeProgramOptions(ProgramOptions programOptions) {
  for(int i = 0; i < NUM_OPTIONS; i++) {
    ProgramOption option = programOptions[i];
    _freeProgramOption(option);
  }
  free(programOptions);
}
