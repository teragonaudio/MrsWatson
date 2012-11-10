//
// ProgramOption.c - MrsWatson
// Created by Nik Reiman on 1/2/12.
// Copyright (c) 2012 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app/ProgramOption.h"
#include "base/FileUtilities.h"
#include "base/StringUtilities.h"
#include "logging/EventLogger.h"
#include "sequencer/AudioSettings.h"

void addNewProgramOption(const ProgramOptions programOptions, const int optionIndex,
  const char* name, const char* help, boolByte hasShortForm, ProgramOptionArgumentType argumentType,
  int defaultValue) {
  ProgramOption programOption = (ProgramOption)malloc(sizeof(ProgramOptionMembers));

  programOption->index = optionIndex;
  programOption->name = newCharStringWithCapacity(STRING_LENGTH_SHORT);
  copyToCharString(programOption->name, name);
  programOption->help = newCharStringWithCapacity(STRING_LENGTH_LONG);
  copyToCharString(programOption->help, help);
  programOption->helpDefaultValue = defaultValue;
  programOption->hasShortForm = hasShortForm;

  programOption->argumentType = argumentType;
  programOption->argument = newCharString();
  programOption->enabled = false;

  programOptions->options[optionIndex] = programOption;
}

static boolByte _isStringShortOption(const char* testString) {
  return (boolByte)(testString != NULL && strlen(testString) == 2 && testString[0] == '-');
}

static boolByte _isStringLongOption(const char* testString) {
  return (boolByte)(testString != NULL && strlen(testString) > 2 && testString[0] == '-' && testString[1] == '-');
}

static ProgramOption _findProgramOption(ProgramOptions programOptions, const char* optionString) {
  ProgramOption potentialMatchOption, optionMatch;
  CharString optionStringWithoutDashes;
  int i;

  if(_isStringShortOption(optionString)) {
    for(i = 0; i < programOptions->numOptions; i++) {
      potentialMatchOption = programOptions->options[i];
      if(potentialMatchOption->hasShortForm && potentialMatchOption->name->data[0] == optionString[1]) {
        return potentialMatchOption;
      }
    }
  }

  if(_isStringLongOption(optionString)) {
    optionMatch = NULL;
    optionStringWithoutDashes = newCharStringWithCapacity(STRING_LENGTH_SHORT);
    strncpy(optionStringWithoutDashes->data, optionString + 2, strlen(optionString) - 2);
    for(i = 0; i < programOptions->numOptions; i++) {
      potentialMatchOption = programOptions->options[i];
      if(isCharStringEqualTo(potentialMatchOption->name, optionStringWithoutDashes, false)) {
        optionMatch = potentialMatchOption;
        break;
      }
    }
    freeCharString(optionStringWithoutDashes);
    return optionMatch;
  }

  // If no option was found, then return null
  return NULL;
}

static boolByte _fillOptionArgument(ProgramOption programOption, int* currentArgc, int argc, char** argv) {
  if(programOption->argumentType == ARGUMENT_TYPE_NONE) {
    return true;
  }
  else if(programOption->argumentType == ARGUMENT_TYPE_OPTIONAL) {
    int potentialNextArgc = *currentArgc + 1;
    if(potentialNextArgc >= argc) {
      return true;
    }
    else {
      char* potentialNextArg = argv[potentialNextArgc];
      // If the next string in the sequence is NOT an argument, we assume it is the optional argument
      if(!_isStringShortOption(potentialNextArg) && !_isStringLongOption(potentialNextArg)) {
        copyToCharString(programOption->argument, potentialNextArg);
        (*currentArgc)++;
        return true;
      }
      else {
        // Otherwise, it is another option, but that's ok
        return true;
      }
    }
  }
  else if(programOption->argumentType == ARGUMENT_TYPE_REQUIRED) {
    int nextArgc = *currentArgc + 1;
    if(nextArgc >= argc) {
      logCritical("Option '%s' requires an argument, but none was given", programOption->name->data);
      return false;
    }
    else {
      char* nextArg = argv[nextArgc];
      if(_isStringShortOption(nextArg) || _isStringLongOption(nextArg)) {
        logCritical("Option '%s' requires an argument, but '%s' is not valid", programOption->name->data, nextArg);
        return false;
      }
      else {
        copyToCharString(programOption->argument, nextArg);
        (*currentArgc)++;
        return true;
      }
    }
  }
  else {
    logInternalError("Unknown argument type '%d'", programOption->argumentType);
    return false;
  }
}

boolByte parseCommandLine(ProgramOptions programOptions, int argc, char** argv) {
  int argumentIndex;
  for(argumentIndex = 1; argumentIndex < argc; argumentIndex++) {
    const ProgramOption option = _findProgramOption(programOptions, argv[argumentIndex]);
    if(option == NULL) {
      logCritical("Invalid option '%s'", argv[argumentIndex]);
      return false;
    }
    else {
      option->enabled = true;
      if(!_fillOptionArgument(option, &argumentIndex, argc, argv)) {
        return false;
      }
    }
  }

  // If we make it to here, return true
  return true;
}

void printProgramOptions(const ProgramOptions programOptions, boolByte withFullHelp, int indentSize) {
  int i;
  for(i = 0; i < programOptions->numOptions; i++) {
    printProgramOption(programOptions->options[i], withFullHelp, indentSize, indentSize);
  }
}

void printProgramOption(const ProgramOption programOption, boolByte withFullHelp, int indentSize, int initialIndent) {
  CharString wrappedHelpString;
  int i;

  if(programOption == NULL) {
    logError("Can't find help for that option. Try running with --help to see all options\n");
    return;
  }

  // Initial argument indent
  for(i = 0; i < initialIndent; i ++) {
    printf(" ");
  }

  // All arguments have a long form, so that will always be printed
  printf("--%s", programOption->name->data);

  if(programOption->hasShortForm) {
    printf(" (or -%c)", programOption->name->data[0]);
  }

  switch(programOption->argumentType) {
    case ARGUMENT_TYPE_REQUIRED:
      printf(" <argument>");
      break;
    case ARGUMENT_TYPE_OPTIONAL:
      printf(" [argument]");
      break;
    case ARGUMENT_TYPE_NONE:
    default:
      break;
  }

  if(programOption->helpDefaultValue != NO_DEFAULT_VALUE) {
    printf(", default value: %d", programOption->helpDefaultValue);
  }

  if(withFullHelp) {
    // Newline and indentation before help
    wrappedHelpString = newCharStringWithCapacity(STRING_LENGTH_LONG);
    wrapString(programOption->help->data, wrappedHelpString->data, initialIndent + indentSize);
    printf("\n%s\n\n", wrappedHelpString->data);
    freeCharString(wrappedHelpString);
  }
  else {
    printf("\n");
  }
}

ProgramOption findProgramOptionFromString(const ProgramOptions programOptions, const CharString string) {
  int i;
  for(i = 0; i < programOptions->numOptions; i++) {
    if(isCharStringEqualTo(string, programOptions->options[i]->name, true)) {
      return programOptions->options[i];
    }
  }
  return NULL;
}

static void _freeProgramOption(ProgramOption programOption) {
  freeCharString(programOption->name);
  freeCharString(programOption->help);
  freeCharString(programOption->argument);
  free(programOption);
}

void freeProgramOptions(ProgramOptions programOptions) {
  ProgramOption option;
  int i;

  for(i = 0; i < programOptions->numOptions; i++) {
    option = programOptions->options[i];
    _freeProgramOption(option);
  }
  free(programOptions->options);
  free(programOptions);
}
