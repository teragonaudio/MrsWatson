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
#include "ProgramOption.h"
#include "EventLogger.h"
#include "StringUtilities.h"
#include "AudioSettings.h"

#define NO_DEFAULT_VALUE -1

static void _addNewProgramOption(const ProgramOptions programOptions, const int optionIndex,
  const char* name, const char* help, boolean hasShortForm, ProgramOptionArgumentType argumentType,
  int defaultValue) {
  ProgramOption programOption = malloc(sizeof(ProgramOptionMembers));

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

  programOptions[optionIndex] = programOption;
}

ProgramOption* newProgramOptions(void) {
  ProgramOptions programOptions = malloc(sizeof(ProgramOptions) * NUM_OPTIONS);

  _addNewProgramOption(programOptions, OPTION_BLOCKSIZE, "blocksize",
    "Blocksize in samples to use for processing. If input source is not an even multiple of the blocksize, then \
empty samples will be added to the last block.",
    true, ARGUMENT_TYPE_REQUIRED, getBlocksize());
  _addNewProgramOption(programOptions, OPTION_CHANNELS, "channels",
    "Number of channels for output source. If the input source's channel count does not match the output source, \
then channels are either copied (mono -> dual mono) or ignored (stereo -> mono, left channel).",
    true, ARGUMENT_TYPE_REQUIRED, getNumChannels());
  _addNewProgramOption(programOptions, OPTION_COLOR_LOGGING, "color",
    "Colored logging output. Argument can be 'light', 'dark', or 'none'. If no argument given, 'dark' is assumed. \
If stderr is a terminal device, color is used automatically unless 'none' is given to this option.",
    false, ARGUMENT_TYPE_OPTIONAL, NO_DEFAULT_VALUE);
  _addNewProgramOption(programOptions, OPTION_DISPLAY_INFO, "display-info",
    "Print information about each plugin in the chain.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);
  _addNewProgramOption(programOptions, OPTION_FILE_TYPES, "file-types",
    "Print a list of supported file types for input/output sources.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);
  _addNewProgramOption(programOptions, OPTION_HELP, "help",
    "Print help",
    true, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);
  _addNewProgramOption(programOptions, OPTION_INPUT_SOURCE, "input",
    "Input source to use for processing, where the file type is determined from the extension. Run with \
--file-types to see a list of supported types. Use '-' to read from stdin.",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);
  _addNewProgramOption(programOptions, OPTION_MIDI_SOURCE, "midi-file",
    "MIDI file to read events from. Required if processing an instrument plugin.",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);
  _addNewProgramOption(programOptions, OPTION_OUTPUT_SOURCE, "output",
    "Output source to write processed data to, where the file type is determined from the extension. Run with \
--file-types to see a list of supported types. Use '-' to write to stdout.",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);
  _addNewProgramOption(programOptions, OPTION_PCM_FILE_NUM_CHANNELS, "pcm-file-num-channels",
    "Number of channels to use when reading raw PCM data.",
    false, ARGUMENT_TYPE_REQUIRED, getNumChannels());
  _addNewProgramOption(programOptions, OPTION_PCM_FILE_SAMPLERATE, "pcm-file-samplerate",
    "Sample rate to use when reading raw PCM data.",
    false, ARGUMENT_TYPE_REQUIRED, (int)getSampleRate());
  _addNewProgramOption(programOptions, OPTION_PLUGIN, "plugin",
    "Plugin(s) to process. Multiple plugins can given in a comma-separated list, in which case they will be \
placed into a chain in the order specified. Instrument plugins must appear first in any chains. Plugins are \
loaded from the standard locations for the OS.",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);
  _addNewProgramOption(programOptions, OPTION_QUIET, "quiet",
    "Only log critical errors.",
    true, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);
  _addNewProgramOption(programOptions, OPTION_SAMPLERATE, "samplerate",
    "Sample rate to use when processing.",
    true, ARGUMENT_TYPE_REQUIRED, (int)getSampleRate());
  _addNewProgramOption(programOptions, OPTION_VERBOSE, "verbose",
    "Verbose logging.",
    true, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);
  _addNewProgramOption(programOptions, OPTION_VERSION, "version",
    "Print full program version and copyright information.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

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
      if(potentialMatchOption->hasShortForm && potentialMatchOption->name->data[0] == optionString[1]) {
        return potentialMatchOption;
      }
    }
  }

  if(_isStringLongOption(optionString)) {
    ProgramOption optionMatch = NULL;
    CharString optionStringWithoutDashes = newCharStringWithCapacity(STRING_LENGTH_SHORT);
    strncpy(optionStringWithoutDashes->data, optionString + 2, strlen(optionString) - 2);
    for(int i = 0; i < NUM_OPTIONS; i++) {
      ProgramOption potentialMatchOption = programOptions[i];
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

static boolean _fillOptionArgument(ProgramOption programOption, int* currentArgc, int argc, char** argv) {
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

boolean parseCommandLine(ProgramOptions programOptions, int argc, char** argv) {
  for(int argumentIndex = 1; argumentIndex < argc; argumentIndex++) {
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

void printProgramOptions(ProgramOptions programOptions) {
  for(int i = 0; i < NUM_OPTIONS; i++) {
    // Don't print out help in help
    if(i == OPTION_HELP) {
      continue;
    }
    printf("  ");
    ProgramOption programOption = programOptions[i];

    if(programOption->hasShortForm) {
      printf("-%c, ", programOption->name->data[0]);
    }

    // All arguments have a long form
    printf("--%s", programOption->name->data);

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

    // Newline and indentation before help
    CharString wrappedHelpString = newCharStringWithCapacity(STRING_LENGTH_LONG);
    wrapStringForTerminal(programOption->help->data, wrappedHelpString->data, 4);
    printf("\n%s\n", wrappedHelpString->data);
    freeCharString(wrappedHelpString);
  }
}

static void _freeProgramOption(ProgramOption programOption) {
  freeCharString(programOption->name);
  freeCharString(programOption->help);
  freeCharString(programOption->argument);
  free(programOption);
}

void freeProgramOptions(ProgramOptions programOptions) {
  for(int i = 0; i < NUM_OPTIONS; i++) {
    ProgramOption option = programOptions[i];
    _freeProgramOption(option);
  }
  free(programOptions);
}
