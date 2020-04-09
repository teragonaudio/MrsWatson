//
// MrsWatsonOptions.h - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
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

#ifndef MrsWatson_MrsWatsonOptions_h
#define MrsWatson_MrsWatsonOptions_h

#include "app/ProgramOption.h"

// Runtime options
typedef enum {
  OPTION_BIT_DEPTH,
  OPTION_BLOCKSIZE,
  OPTION_CHANNELS,
  OPTION_COLOR_LOGGING,
  OPTION_COLOR_TEST,
  OPTION_CONFIG_FILE,
  OPTION_DISPLAY_INFO,
  OPTION_EDITOR,
  OPTION_ENDIAN,
  OPTION_ERROR_REPORT,
  OPTION_HELP,
  OPTION_INPUT_SOURCE,
  OPTION_LIST_FILE_TYPES,
  OPTION_LIST_PLUGINS,
  OPTION_LOG_FILE,
  OPTION_LOG_LEVEL,
  OPTION_MAX_TIME,
  OPTION_MIDI_SOURCE,
  OPTION_MIDI_TRACK,
  OPTION_OUTPUT_SOURCE,
  OPTION_PARAMETER,
  OPTION_PLUGIN,
  OPTION_PLUGIN_ROOT,
  OPTION_QUIET,
  OPTION_REALTIME,
  OPTION_SAMPLE_RATE,
  OPTION_TEMPO,
  OPTION_TIME_SIGNATURE,
  OPTION_VERBOSE,
  OPTION_VERSION,
  OPTION_ZEBRA_SIZE,
  NUM_OPTIONS
} ProgramOptionIndex;

ProgramOptions newMrsWatsonOptions(void);
void printMrsWatsonQuickstart(const char *argvName);

#endif
