//
// MrsWatson.h - MrsWatson
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

#ifndef MrsWatson_MrsWatson_h
#define MrsWatson_MrsWatson_h

#define PROGRAM_NAME "MrsWatson"
#define VENDOR_NAME "Teragon Audio"

#define VERSION_MAJOR 0
#define VERSION_MINOR 9
#define VERSION_PATCH 2

#define OFFICIAL_WEBSITE "http://www.teragonaudio.com"
#define PROJECT_WEBSITE "https://github.com/teragonaudio/mrswatson"
#define SUPPORT_WEBSITE "https://github.com/teragonaudio/mrswatson/issues"
#define SUPPORT_EMAIL "support@teragonaudio.com"

#define LICENSE_STRING "Redistribution and use in source and binary forms, with or without " \
"modification, are permitted provided that the following conditions are met:\n\n" \
"* Redistributions of source code must retain the above copyright notice, this list of " \
"conditions and the following disclaimer.\n" \
"* Redistributions in binary form must reproduce the above copyright notice, this list of " \
"conditions and the following disclaimer in the documentation and/or other materials " \
"provided with the distribution.\n\n" \
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY " \
"EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF " \
"MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE " \
"COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, " \
"EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE " \
"GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED " \
"AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING " \
"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED " \
"OF THE POSSIBILITY OF SUCH DAMAGE."

// Runtime options
typedef enum {
  OPTION_BLOCKSIZE,
  OPTION_CHANNELS,
  OPTION_COLOR_LOGGING,
  OPTION_DISPLAY_INFO,
  OPTION_ERROR_REPORT,
  OPTION_HELP,
  OPTION_INPUT_SOURCE,
  OPTION_LIST_FILE_TYPES,
  OPTION_LIST_PLUGINS,
  OPTION_LOG_FILE,
  OPTION_MIDI_SOURCE,
  OPTION_OPTIONS,
  OPTION_OUTPUT_SOURCE,
  OPTION_PCM_NUM_CHANNELS,
  OPTION_PCM_SAMPLE_RATE,
  OPTION_PLUGIN,
  OPTION_PLUGIN_ROOT,
  OPTION_QUIET,
  OPTION_SAMPLE_RATE,
  OPTION_TAIL_TIME,
  OPTION_TEMPO,
  OPTION_TIME_DIVISION,
  OPTION_TIME_SIGNATURE_TOP,
  OPTION_TIME_SIGNATURE_BOTTOM,
  OPTION_VERBOSE,
  OPTION_VERSION,
  OPTION_ZEBRA_SIZE,
  NUM_OPTIONS
} ProgramOptionIndex;

// Exit result codes
typedef enum {
  RETURN_CODE_SUCCESS,
  RETURN_CODE_NOT_RUN,
  RETURN_CODE_INVALID_ARGUMENT,
  RETURN_CODE_MISSING_REQUIRED_OPTION,
  RETURN_CODE_IO_ERROR,
  RETURN_CODE_PLUGIN_ERROR,
  RETURN_CODE_INVALID_PLUGIN_CHAIN,
  RETURN_CODE_UNSUPPORTED_FEATURE,
  // This return code should always be right before the last one. It is not
  // actually used, but instead we add the signal number to it and exit with
  // that code instead.
  RETURN_CODE_SIGNAL,
  NUM_RETURN_CODES
} ReturnCodes;

int mrsWatsonMain(int argc, char* argv[]);

#endif
