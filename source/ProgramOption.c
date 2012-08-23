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

  _addNewProgramOption(programOptions, OPTION_LIST_FILE_TYPES, "list-file-types",
    "Print a list of supported file types for input/output sources.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_HELP, "help",
    "Print full program help (this screen), or just the help for a single argument.",
    true, ARGUMENT_TYPE_OPTIONAL, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_INPUT_SOURCE, "input",
    "Input source to use for processing, where the file type is determined from the extension. Run with \
--list-file-types to see a list of supported types. Use '-' to read from stdin.",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_LIST_PLUGINS, "list-plugins",
    "List available plugins. Useful for determining if a plugin can be 'seen'.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_MIDI_SOURCE, "midi-file",
    "MIDI file to read events from. Required if processing an instrument plugin.",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_LOG_FILE, "log-file",
    "Save logging output to the given file instead of the terminal's standard error.",
    false, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_OPTIONS, "options",
    "Show program options and their default values.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_OUTPUT_SOURCE, "output",
    "Output source to write processed data to, where the file type is determined from the extension. Run with \
--list-file-types to see a list of supported types. Use '-' to write to stdout.",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_PCM_NUM_CHANNELS, "pcm-num-channels",
    "Number of channels to use when reading raw PCM data. If not given, defaults to the global channel count \
from the --channels option.",
    false, ARGUMENT_TYPE_REQUIRED, getNumChannels());

  _addNewProgramOption(programOptions, OPTION_PCM_SAMPLE_RATE, "pcm-sample-rate",
    "Sample rate to use when reading raw PCM data. If not given, defaults to the global sample rate from the \
--sample-rate option.",
    false, ARGUMENT_TYPE_REQUIRED, (int)getSampleRate());

  _addNewProgramOption(programOptions, OPTION_PLUGIN, "plugin",
    "Plugin(s) to process. Multiple plugins can given in a semicolon-separated list, in which case they will be \
placed into a chain in the order specified. Instrument plugins must appear first in any chains. Plugins are searched \
for in the --plugin-root directory, the current directory, and the standard locations for the OS. File extensions are \
added automatically to plugin names. Each plugin may be followed by a comma with a program to be loaded, which should \
be of the corresponding file format for the respective plugin.\
\nExample: --plugin 'AutoTune,KayneWest.fxp;Compressor,SoftKnee.fxp;Limiter'",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_PLUGIN_ROOT, "plugin-root",
    "Custom non-system directory to use when searching for plugins.",
    false, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_QUIET, "quiet",
    "Only log critical errors.",
    true, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_SAMPLE_RATE, "sample-rate",
    "Sample rate to use when processing.",
    true, ARGUMENT_TYPE_REQUIRED, (int)getSampleRate());

  _addNewProgramOption(programOptions, OPTION_TAIL_TIME, "tail-time",
    "Continue processing for up to <argument> extra milliseconds after input source is finished, in addition \
to any tail time requested by plugins in the chain. If any plugins in chain the require tail time, the largest \
value will be used and added to <argument>.",
    false, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_TEMPO, "tempo",
    "Tempo to use when processing.",
    false, ARGUMENT_TYPE_REQUIRED, (int)getTempo());

  _addNewProgramOption(programOptions, OPTION_TIME_SIGNATURE_TOP, "time-signature-top",
    "Set the numerator of the time signature, which determines the number of beats per measure.",
    false, ARGUMENT_TYPE_REQUIRED, getTimeSignatureBeatsPerMeasure());

  _addNewProgramOption(programOptions, OPTION_TIME_SIGNATURE_BOTTOM, "time-signature-bottom",
    "Set the denominator of the time signature, which determines the value of a quarter note.",
    false, ARGUMENT_TYPE_REQUIRED, getTimeSignatureNoteValue());

  _addNewProgramOption(programOptions, OPTION_VERBOSE, "verbose",
    "Verbose logging. Logging output is printed in the following form:\n\
(Level) (Frames processed) (Elapsed time in ms) (Logging message)",
    true, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_VERSION, "version",
    "Print full program version and copyright information.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  _addNewProgramOption(programOptions, OPTION_ZEBRA_SIZE, "zebra-size",
    "Alternate logging output colors every <argument> samples.",
    false, ARGUMENT_TYPE_REQUIRED, (int)getSampleRate());

  return programOptions;
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
    for(i = 0; i < NUM_OPTIONS; i++) {
      potentialMatchOption = programOptions[i];
      if(potentialMatchOption->hasShortForm && potentialMatchOption->name->data[0] == optionString[1]) {
        return potentialMatchOption;
      }
    }
  }

  if(_isStringLongOption(optionString)) {
    optionMatch = NULL;
    optionStringWithoutDashes = newCharStringWithCapacity(STRING_LENGTH_SHORT);
    strncpy(optionStringWithoutDashes->data, optionString + 2, strlen(optionString) - 2);
    for(i = 0; i < NUM_OPTIONS; i++) {
      potentialMatchOption = programOptions[i];
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

void printProgramQuickHelp(const char* argvName) {
  const char *programBasename = getFileBasename(argvName);
  printf("Quickstart for effects: %s -p <plugin> -i <input file> -o <output>\n", programBasename);
  printf("Quickstart for instruments: %s -p <name> -m <midi file> -o <output>\n", programBasename);
  printf("\n");
}

void printProgramOptions(const ProgramOptions programOptions, boolByte withFullHelp, int indentSize) {
  int i;
  for(i = 0; i < NUM_OPTIONS; i++) {
    printProgramOption(programOptions[i], withFullHelp, indentSize, indentSize);
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
    wrapStringForTerminal(programOption->help->data, wrappedHelpString->data, initialIndent + indentSize);
    printf("\n%s\n\n", wrappedHelpString->data);
    freeCharString(wrappedHelpString);
  }
  else {
    printf("\n");
  }
}

ProgramOption findProgramOptionFromString(const ProgramOptions programOptions, const CharString string) {
  int i;
  for(i = 0; i < NUM_OPTIONS; i++) {
    if(isCharStringEqualTo(string, programOptions[i]->name, true)) {
      return programOptions[i];
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

  for(i = 0; i < NUM_OPTIONS; i++) {
    option = programOptions[i];
    _freeProgramOption(option);
  }
  free(programOptions);
}
