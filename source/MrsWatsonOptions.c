//
// MrsWatsonOptions.c - MrsWatson
// Created by Nik Reiman on 10/23/12.
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

#include <stdlib.h>
#include <stdio.h>
#include "MrsWatsonOptions.h"
#include "AudioSettings.h"
#include "FileUtilities.h"

ProgramOptions newMrsWatsonOptions(void) {
  ProgramOptions programOptions = malloc(sizeof(ProgramOptions));
  programOptions->options = malloc(sizeof(ProgramOption) * NUM_OPTIONS);
  programOptions->numOptions = NUM_OPTIONS;

  addNewProgramOption(programOptions, OPTION_BLOCKSIZE, "blocksize",
    "Blocksize in frames to use for processing. If input source is not an even multiple of the blocksize, then \
empty frames will be added to the last block.",
    true, ARGUMENT_TYPE_REQUIRED, getBlocksize());

  addNewProgramOption(programOptions, OPTION_CHANNELS, "channels",
    "Number of channels for output source. If the input source specifies a channel count, then that value \
will be override the one set by this option.",
    true, ARGUMENT_TYPE_REQUIRED, getNumChannels());

  addNewProgramOption(programOptions, OPTION_COLOR_LOGGING, "color",
    "Colored logging output. Argument can be 'auto', 'force', or 'none'. If no argument given, 'force' is assumed. \
If attached to a terminal device, color is used automatically unless 'none' is given to this option.",
    false, ARGUMENT_TYPE_OPTIONAL, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_COLOR_TEST, "color-test",
    "Run an ANSI color test.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_DISPLAY_INFO, "display-info",
    "Print information about each plugin in the chain.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_ERROR_REPORT, "error-report",
    "Generate an error report zipfile on the desktop.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_LIST_FILE_TYPES, "list-file-types",
    "Print a list of supported file types for input/output sources.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_HELP, "help",
    "Print full program help (this screen), or just the help for a single argument.",
    true, ARGUMENT_TYPE_OPTIONAL, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_INPUT_SOURCE, "input",
    "Input source to use for processing, where the file type is determined from the extension. Run with \
--list-file-types to see a list of supported types. Use '-' to read from stdin.",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_LIST_PLUGINS, "list-plugins",
    "List available plugins. Useful for determining if a plugin can be 'seen'.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_LOG_FILE, "log-file",
    "Save logging output to the given file instead of the terminal's standard error.",
    false, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_MIDI_SOURCE, "midi-file",
    "MIDI file to read events from. Required if processing an instrument plugin.",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_OPTIONS, "options",
    "Show program options and their default values.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_OUTPUT_SOURCE, "output",
    "Output source to write processed data to, where the file type is determined from the extension. Run with \
--list-file-types to see a list of supported types. Use '-' to write to stdout.",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_PLUGIN, "plugin",
    "Plugin(s) to process. Multiple plugins can given in a semicolon-separated list, in which case they will be \
placed into a chain in the order specified. Instrument plugins must appear first in any chains. Plugins are searched \
for in the --plugin-root directory, the current directory, and the standard locations for the OS. File extensions are \
added automatically to plugin names. Each plugin may be followed by a comma with a program to be loaded, which should \
be of the corresponding file format for the respective plugin.\
\nExample: --plugin 'AutoTune,KayneWest.fxp;Compressor,SoftKnee.fxp;Limiter'",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_PLUGIN_ROOT, "plugin-root",
    "Custom non-system directory to use when searching for plugins.",
    false, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_QUIET, "quiet",
    "Only log critical errors.",
    true, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_SAMPLE_RATE, "sample-rate",
    "Sample rate to use when processing. If the input source specifies its own sample rate, that value will override \
the one set by this option.",
    true, ARGUMENT_TYPE_REQUIRED, (int)getSampleRate());

  addNewProgramOption(programOptions, OPTION_TAIL_TIME, "tail-time",
    "Continue processing for up to <argument> extra milliseconds after input source is finished, in addition \
to any tail time requested by plugins in the chain. If any plugins in chain the require tail time, the largest \
value will be used and added to <argument>.",
    false, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_TEMPO, "tempo",
    "Tempo to use when processing.",
    false, ARGUMENT_TYPE_REQUIRED, (int)getTempo());

  addNewProgramOption(programOptions, OPTION_TIME_DIVISION, "time-division",
    "Time division in PPQ. If using a MIDI file input, this value will be ignored.",
    false, ARGUMENT_TYPE_REQUIRED, DEFAULT_TIME_DIVISION);

  addNewProgramOption(programOptions, OPTION_TIME_SIGNATURE_TOP, "time-signature-top",
    "Set the numerator of the time signature, which determines the number of beats per measure.",
    false, ARGUMENT_TYPE_REQUIRED, getTimeSignatureBeatsPerMeasure());

  addNewProgramOption(programOptions, OPTION_TIME_SIGNATURE_BOTTOM, "time-signature-bottom",
    "Set the denominator of the time signature, which determines the value of a quarter note.",
    false, ARGUMENT_TYPE_REQUIRED, getTimeSignatureNoteValue());

  addNewProgramOption(programOptions, OPTION_VERBOSE, "verbose",
    "Verbose logging. Logging output is printed in the following form:\n\
(Level) (Frames processed) (Elapsed time in ms) (Logging message)",
    true, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_VERSION, "version",
    "Print full program version and copyright information.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  addNewProgramOption(programOptions, OPTION_ZEBRA_SIZE, "zebra-size",
    "Alternate logging output colors every <argument> frames.",
    false, ARGUMENT_TYPE_REQUIRED, (int)getSampleRate());

  return programOptions;
}

void printProgramQuickHelp(const char* argvName) {
  const char *programBasename = getFileBasename(argvName);
  printf("Quickstart for effects: %s -p <plugin> -i <input file> -o <output>\n", programBasename);
  printf("Quickstart for instruments: %s -p <name> -m <midi file> -o <output>\n", programBasename);
  printf("\n");
}
