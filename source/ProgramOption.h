//
// ProgramOption.h - MrsWatson
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
  int helpDefaultValue;
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
  OPTION_LIST_FILE_TYPES,
  OPTION_LIST_PLUGINS,
  OPTION_MIDI_SOURCE,
  OPTION_OUTPUT_SOURCE,
  OPTION_PCM_NUM_CHANNELS,
  OPTION_PCM_SAMPLE_RATE,
  OPTION_PLUGIN,
  OPTION_PLUGIN_ROOT,
  OPTION_QUIET,
  OPTION_SAMPLE_RATE,
  OPTION_TAIL_TIME,
  OPTION_TEMPO,
  OPTION_TIME_SIGNATURE_TOP,
  OPTION_TIME_SIGNATURE_BOTTOM,
  OPTION_VERBOSE,
  OPTION_VERSION,
  OPTION_ZEBRA_SIZE,
  NUM_OPTIONS
} ProgramOptionIndex;

typedef ProgramOptionMembers* ProgramOption;
typedef ProgramOption* ProgramOptions;

ProgramOptions newProgramOptions(void);
boolean parseCommandLine(ProgramOptions programOptions, int argc, char** argv);
const ProgramOption findProgramOptionFromString(const ProgramOptions programOptions, const CharString string);
void printProgramQuickHelp(const char* argvName);
void printProgramOptionsHelp(const ProgramOptions programOptions, int indentSize);
void printProgramOptionHelp(const ProgramOption programOption, int indentSize, int initialIndent);
void freeProgramOptions(ProgramOptions programOptions);

#endif
