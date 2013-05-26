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

#include "audio/AudioSettings.h"
#include "base/FileUtilities.h"

#include "MrsWatsonOptions.h"

ProgramOptions newMrsWatsonOptions(void) {
  ProgramOptions options = newProgramOptions(NUM_OPTIONS);

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_BLOCKSIZE, "blocksize",
    "Blocksize in frames to use for processing. If input source is not an even multiple of the blocksize, then \
empty frames will be added to the last block.",
    true, kProgramOptionArgumentTypeRequired, getBlocksize()));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_CHANNELS, "channels",
    "Number of channels for output source. If the input source specifies a channel count, then that value \
will be override the one set by this option.",
    true, kProgramOptionArgumentTypeRequired, getNumChannels()));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_COLOR_LOGGING, "color",
    "Colored logging output. Argument can be 'auto', 'force', or 'none'. If no argument given, 'force' is assumed. \
If attached to a terminal device, color is used automatically unless 'none' is given to this option.",
    false, kProgramOptionArgumentTypeOptional, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_COLOR_TEST, "color-test",
    "Run a test of all color output combinations.",
    false, kProgramOptionArgumentTypeNone, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_CONFIG_FILE, "config-file",
    "Load options from a configuration file. The file will be read *after* other \
options have been parsed, so any options given on the command line will be overriden \
by those from the file. The file should be plain text, and one argument per line, \
like so:\n\n\
\t--plugin-root\n\
\t/path/to/my/plugins\n\
\t--verbose",
    false, kProgramOptionArgumentTypeRequired, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_DISPLAY_INFO, "display-info",
    "Print information about each plugin in the chain.",
    false, kProgramOptionArgumentTypeNone, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_ERROR_REPORT, "error-report",
    "Generate an error report zipfile on the desktop.",
    false, kProgramOptionArgumentTypeNone, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_LIST_FILE_TYPES, "list-file-types",
    "Print a list of supported file types for input/output sources.",
    false, kProgramOptionArgumentTypeNone, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_HELP, "help",
    "Prints a summary of options and their default settings. Can be used with \
'full' as an argument to print extended help for all options.",
    true, kProgramOptionArgumentTypeOptional, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_INPUT_SOURCE, "input",
    "Input source to use for processing, where the file type is determined from the extension. Run with \
--list-file-types to see a list of supported types. Use '-' to read from stdin.",
    true, kProgramOptionArgumentTypeRequired, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_LIST_PLUGINS, "list-plugins",
    "List available plugins. Useful for determining if a plugin can be 'seen'.",
    false, kProgramOptionArgumentTypeNone, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_LOG_FILE, "log-file",
    "Save logging output to the given file instead of the terminal's standard error.",
    false, kProgramOptionArgumentTypeRequired, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_LOG_LEVEL, "log-level",
    "Logging level to use. Options include: debug, info (default), warn, error. \
Critical errors are always logged.",
    false, kProgramOptionArgumentTypeRequired, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_MAX_TIME, "max-time",
    "Force processing to stop after <argument> milliseconds, regardless of the \
input source length. Mostly useful when using internal plugins as sources. Note \
that --tail-time is still applied as normal after this limit is reached.",
    false, kProgramOptionArgumentTypeRequired, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_MIDI_SOURCE, "midi-file",
    "MIDI file to read events from. Required if processing an instrument plugin.",
    true, kProgramOptionArgumentTypeRequired, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_OUTPUT_SOURCE, "output",
    "Output source to write processed data to, where the file type is determined \
from the extension. Run with --list-file-types to see a list of supported types. \
Use '-' to write to stdout. If not given, then defaults to 'out.wav'.",
    true, kProgramOptionArgumentTypeOptional, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_PLUGIN, "plugin",
    "Plugin(s) to process. Multiple plugins can given in a semicolon-separated list, in which case they will be \
placed into a chain in the order specified. Instrument plugins must appear first in any chains. Plugins are searched \
for in the --plugin-root directory, the current directory, and the standard locations for the OS. File extensions are \
added automatically to plugin names. Each plugin may be followed by a comma with a program to be loaded, which should \
be of the corresponding file format for the respective plugin. For shell plugins (like Waves), use --display-info to \
get a list of sub-plugin ID's and then use a colon to indicate which plugin to load. Examples:\n\n\
\t--plugin LFX-1310\n\
\t--plugin 'AutoTune,KayneWest.fxp;Compressor,SoftKnee.fxp;Limiter'\n\
\t--plugin 'WavesShell-VST' --display-info (list shell sub-plugins)\n\
\t--plugin 'WavesShell-VST:IDFX' (load a shell plugins)",
    true, kProgramOptionArgumentTypeRequired, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_PLUGIN_ROOT, "plugin-root",
    "Custom non-system directory to use when searching for plugins. Will be searched \
  before system directories if given.",
    false, kProgramOptionArgumentTypeRequired, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_QUIET, "quiet",
    "Only log critical errors.",
    true, kProgramOptionArgumentTypeNone, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_SAMPLE_RATE, "sample-rate",
    "Sample rate to use when processing. If the input source specifies its own sample rate, that value will override \
the one set by this option.",
    true, kProgramOptionArgumentTypeRequired, (int)getSampleRate()));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_TAIL_TIME, "tail-time",
    "Continue processing for up to <argument> extra milliseconds after input source is finished, in addition \
to any tail time requested by plugins in the chain. If any plugins in chain the require tail time, the largest \
value will be used and added to <argument>.",
    false, kProgramOptionArgumentTypeRequired, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_TEMPO, "tempo",
    "Tempo to use when processing.",
    false, kProgramOptionArgumentTypeRequired, (int)getTempo()));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_TIME_SIGNATURE_TOP, "time-signature-top",
    "Set the numerator of the time signature, which determines the number of beats per measure.",
    false, kProgramOptionArgumentTypeRequired, getTimeSignatureBeatsPerMeasure()));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_TIME_SIGNATURE_BOTTOM, "time-signature-bottom",
    "Set the denominator of the time signature, which determines the value of a quarter note.",
    false, kProgramOptionArgumentTypeRequired, getTimeSignatureNoteValue()));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_VERBOSE, "verbose",
    "Verbose logging. Logging output is printed in the following form:\n\
(Level) (Frames processed) (Elapsed time in ms) (Logging message)",
    true, kProgramOptionArgumentTypeNone, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_VERSION, "version",
    "Print full program version and copyright information.",
    false, kProgramOptionArgumentTypeNone, NO_DEFAULT_VALUE));

  programOptionsAdd(options, newProgramOptionWithValues(OPTION_ZEBRA_SIZE, "zebra-size",
    "Alternate logging output colors every <argument> frames.",
    false, kProgramOptionArgumentTypeRequired, (int)getSampleRate()));

  return options;
}

void printMrsWatsonQuickstart(const char* argvName) {
  const char *programBasename = getFileBasename(argvName);
  printf("Run with '--help full' to see extended help for all options.\n");
  printf("Quickstart for effects: %s -p <plugin> -i <input file> -o <output>\n", programBasename);
  printf("Quickstart for instruments: %s -p <name> -m <midi file> -o <output>\n", programBasename);
  printf("\n");
}
