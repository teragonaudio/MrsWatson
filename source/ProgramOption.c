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
#include "EventLogger.h"

static void _addNewProgramOption(const ProgramOptions programOptions, const int index,
  const char* name, const char* help, boolean hasShortForm, ProgramOptionArgumentType argumentType) {
  ProgramOption programOption = malloc(sizeof(ProgramOptionMembers));

  programOption->index = index;
  programOption->name = newCharStringShort();
  strncpy(programOption->name, name, STRING_LENGTH_SHORT);
  programOption->help = newCharStringLong();
  strncpy(programOption->help, help, STRING_LENGTH_LONG);
  programOption->hasShortForm = hasShortForm;
  
  programOption->argumentType = argumentType;
  programOption->argument = newCharString();
  programOption->enabled = false;

  programOptions[index] = programOption;
}

ProgramOption* newProgramOptions(void) {
  ProgramOptions programOptions = malloc(sizeof(ProgramOptions) * NUM_OPTIONS);

  // TODO: Expand help for options
  _addNewProgramOption(programOptions, OPTION_PLUGIN, "plugin", "Plugin(s) to process", true, ARGUMENT_TYPE_REQUIRED);
  _addNewProgramOption(programOptions, OPTION_DISPLAY_INFO, "info", "Print information about the plugin(s)", false, ARGUMENT_TYPE_NONE);
  _addNewProgramOption(programOptions, OPTION_INPUT_SOURCE, "input", "Input source", true, ARGUMENT_TYPE_REQUIRED);
  _addNewProgramOption(programOptions, OPTION_HELP, "help", "Print help", true, ARGUMENT_TYPE_NONE);
  _addNewProgramOption(programOptions, OPTION_VERSION, "version", "Print version and copyright information", false, ARGUMENT_TYPE_NONE);
  _addNewProgramOption(programOptions, OPTION_COLOR_LOGGING, "color", "Color-coded logging output", false, ARGUMENT_TYPE_OPTIONAL);
  _addNewProgramOption(programOptions, OPTION_VERBOSE, "verbose", "Verbose logging", true, ARGUMENT_TYPE_NONE);
  _addNewProgramOption(programOptions, OPTION_QUIET, "quiet", "Only log critical errors", true, ARGUMENT_TYPE_NONE);

  return programOptions;
}

static boolean _isStringShortOption(const char* testString) {
  return (testString != NULL && strlen(testString) == 2 && testString[0] == '-');
}

static boolean _isStringLongOption(const char* testString) {
  return (testString != NULL && strlen(testString) > 2 && testString[0] == '-' && testString[1] == '-');  
}

static ProgramOption _findProgramOption(ProgramOptions programOptions, const char* optionString) {
  if(_isStringShortOption(optionString)) {
    for(int i = 0; i < NUM_OPTIONS; i++) {
      ProgramOption potentialMatchOption = programOptions[i];
      if(potentialMatchOption->hasShortForm && potentialMatchOption->name[0] == optionString[1]) {
        return potentialMatchOption;
      }
    }
  }

  if(_isStringLongOption(optionString)) {
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

  // If no option was found, then return null
  return NULL;
}

static boolean _fillOptionArgument(ProgramOption programOption, int* currentArgc, int argc, char** argv) {
  if(programOption->argumentType == ARGUMENT_TYPE_NONE) {
    return false;
  }
  else if(programOption->argumentType == ARGUMENT_TYPE_OPTIONAL) {
    int potentialNextArgc = *currentArgc + 1;
    if(potentialNextArgc >= argc) {
      return false;
    }
    else {
      char* potentialNextArg = argv[potentialNextArgc];
      // If the next string in the sequence is NOT an argument, we assume it is the optional argument
      if(!_isStringShortOption(potentialNextArg) && !_isStringLongOption(potentialNextArg)) {
        strncpy(programOption->argument, potentialNextArg, STRING_LENGTH);
        (*currentArgc)++;
        return true;
      }
      else {
        return false;
      }
    }
  }
  else if(programOption->argumentType == ARGUMENT_TYPE_REQUIRED) {
    int nextArgc = *currentArgc + 1;
    if(nextArgc >= argc) {
      // TODO: It would be nice to actually print the option name here
      logCritical("Option '%s' requires an argument, but none was given", programOption->name);
      return false;
    }
    else {
      char* nextArg = argv[nextArgc];
      if(_isStringShortOption(nextArg) || _isStringLongOption(nextArg)) {
        logCritical("Option '%s' requires an argument, but it does not seem to be valid", programOption->name);
        return false;
      }
      else {
        strncpy(programOption->argument, nextArg, STRING_LENGTH);
        (*currentArgc)++;
        return true;
      }
    }
  }
  else {
    logCritical("Internal error while parsing arguments. Please report!");
    return false;
  }
}

boolean parseCommandLine(ProgramOptions programOptions, int argc, char** argv) {
  for(int argumentIndex = 1; argumentIndex < argc; argumentIndex++) {
    const ProgramOption option = _findProgramOption(programOptions, argv[argumentIndex]);
    if(option == NULL) {
      // Invalid option, bail out
      return false;
    }
    else {
      option->enabled = true;
      _fillOptionArgument(option, &argumentIndex, argc, argv);
    }
  }

  // If we make it to here, return true
  return true;
}

void printProgramOptions(ProgramOptions programOptions) {
  // TODO: Instead of just printing out all options, they should be alphabetized. This is nice.
  for(int i = 0; i < NUM_OPTIONS; i++) {
    printf("  ");
    ProgramOption programOption = programOptions[i];

    if(programOption->hasShortForm) {
      printf("-%c, ", programOption->name[0]);
    }

    // All arguments have a long form
    printf("--%s", programOption->name);

    switch(programOption->argumentType) {
      case ARGUMENT_TYPE_REQUIRED:
        printf(" (argument)");
        break;
      case ARGUMENT_TYPE_OPTIONAL:
        printf(" [argument]");
        break;
      case ARGUMENT_TYPE_NONE:
      default:
        break;
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
  free(programOption->argument);
  free(programOption);
}

void freeProgramOptions(ProgramOptions programOptions) {
  for(int i = 0; i < NUM_OPTIONS; i++) {
    ProgramOption option = programOptions[i];
    _freeProgramOption(option);
  }
  free(programOptions);
}
