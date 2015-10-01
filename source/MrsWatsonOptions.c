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

#include <stdio.h>

#include "audio/AudioSettings.h"
#include "base/File.h"

#include "MrsWatsonOptions.h"

ProgramOptions newMrsWatsonOptions(void)
{
    ProgramOptions options = newProgramOptions(NUM_OPTIONS);

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_BIT_DEPTH,
                          "bit-depth",
                          "Bit depth to use for processing. If the input source specifies a bit depth, \
than that value will override the one set by this option. Valid values for bit depth include: 8, 16, 24, 32.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeNumber,
                          kProgramOptionArgumentTypeRequired));
    programOptionsSetNumber(options, OPTION_BIT_DEPTH, (const float)getBitDepth());
    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_BLOCKSIZE,
                          "blocksize",
                          "Blocksize in frames to use for processing. If input source is not an even \
multiple of the blocksize, then empty frames will be added to the last block.",
                          HAS_SHORT_FORM,
                          kProgramOptionTypeNumber,
                          kProgramOptionArgumentTypeRequired));
    programOptionsSetNumber(options, OPTION_BLOCKSIZE, (const float)getBlocksize());

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_CHANNELS,
                          "channels",
                          "Number of channels for output source. If the input source specifies a channel \
count, then that value will override the one set by this option.",
                          HAS_SHORT_FORM,
                          kProgramOptionTypeNumber,
                          kProgramOptionArgumentTypeRequired));
    programOptionsSetNumber(options, OPTION_CHANNELS, (const float)getNumChannels());

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_COLOR_LOGGING,
                          "color",
                          "Colored logging output. Argument can be 'auto', 'force', or 'none'. If no \
argument given, 'force' is assumed. If attached to a terminal device, color is \
used automatically unless 'none' is given to this option.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeOptional));
    programOptionsSetCString(options, OPTION_COLOR_LOGGING, "auto");

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_COLOR_TEST,
                          "color-test",
                          "Run a test of all color output combinations.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeEmpty,
                          kProgramOptionArgumentTypeNone));
    options->options[OPTION_COLOR_TEST]->hideInHelp = true;

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_CONFIG_FILE,
                          "config-file",
                          "Load options from a configuration file. The file will be read *after* other \
options have been parsed, so any options given on the command line will be overriden \
by those from the file. The file should be plain text, and one argument per line, \
like so:\n\n\
\t--plugin-root\n\
\t/path/to/my/plugins\n\
\t--verbose",
                          NO_SHORT_FORM,
                          kProgramOptionTypeEmpty,
                          kProgramOptionArgumentTypeRequired));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_DISPLAY_INFO,
                          "display-info",
                          "Print information about each plugin in the chain.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeEmpty,
                          kProgramOptionArgumentTypeNone));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_ENDIAN,
                          "endian",
                          "Endian-ness to use when reading and writing raw PCM data. Recognized values \
are \"big\" or \"little\".",
                          NO_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeNone));
    programOptionsSetCString(options, OPTION_ENDIAN, "little");

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_EDITOR,
                          "editor",
                          "EXPERIMENTAL: Show plugin editor. When the editor window is closed, \
MrsWatson will exit. Changes made in the editor are not sent to the plugin. This feature is mostly \
intended for debugging plugin GUIs.",
                          false,
                          kProgramOptionTypeEmpty,
                          kProgramOptionArgumentTypeNone));
    options->options[OPTION_EDITOR]->hideInHelp = true;

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_ERROR_REPORT,
                          "error-report",
                          "Generate an error report zipfile on the desktop.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeNone));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_LIST_FILE_TYPES,
                          "list-file-types",
                          "Print a list of supported file types for input/output sources.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeNone));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_HELP,
                          "help",
                          "With no arguments, prints a summary of options and their default settings. \
Otherwise, extended help can be printed for an individual option given by \
[argument], or use 'full' to print extended help for all options.",
                          HAS_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeOptional));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_INPUT_SOURCE,
                          "input",
                          "Input source to use for processing, where the file type is determined from \
the extension. Run with --list-file-types to see a list of supported types. Use \
'-' to read from stdin.",
                          HAS_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeRequired));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_LIST_PLUGINS,
                          "list-plugins",
                          "List available plugins. Useful for determining if a plugin can be 'seen'.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeEmpty,
                          kProgramOptionArgumentTypeNone));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_LOG_FILE,
                          "log-file",
                          "Save logging output to the given file instead of the terminal's standard error.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeRequired));
    programOptionsSetCString(options, OPTION_LOG_FILE, "log.txt");

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_LOG_LEVEL,
                          "log-level",
                          "Logging level to use. Options include: debug, info, warn, error. Critical \
errors are always logged to console regardless of this setting.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeRequired));
    programOptionsSetCString(options, OPTION_LOG_LEVEL, "info");

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_MAX_TIME,
                          "max-time",
                          "Force processing to stop after <argument> milliseconds, regardless of the \
input source length. Mostly useful when using internal plugins as sources.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeNumber,
                          kProgramOptionArgumentTypeRequired));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_MIDI_SOURCE,
                          "midi-file",
                          "MIDI file to read events from. Required if processing an instrument plugin.",
                          HAS_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeRequired));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_OUTPUT_SOURCE,
                          "output",
                          "Output source to write processed data to, where the file type is determined \
from the extension. Run with --list-file-types to see a list of supported types. \
Use '-' to write to stdout..",
                          HAS_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeOptional));
    programOptionsSetCString(options, OPTION_OUTPUT_SOURCE, "out.wav");

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_PARAMETER,
                          "parameter",
                          "Set a parameter in a plugin. May be specified multiple times, but can only \
set parameters for the first plugin in a chain. Parameter indexes for plugins \
can be found with the --display-info option. Use comma-separated arguments for \
index/value, for example:\n\n\
\t--parameter 1,0.3 --parameter 0,0.75",
                          NO_SHORT_FORM,
                          kProgramOptionTypeList,
                          kProgramOptionArgumentTypeRequired));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_PLUGIN,
                          "plugin",
                          "Plugin(s) to process. Multiple plugins can given in a semicolon-separated \
list, in which case they will be placed into a chain in the order specified. \
Note that if you specify multiple plugins, you will have to put the argument \
in quotes or else the shell may mis-interpret it as multiple commands. \
Instrument plugins must appear first in any chains. Plugins are searched for in \
the --plugin-root directory, the current directory, and the standard locations \
for the OS. File extensions are added automatically to plugin names. Each plugin \
may be followed by a comma with a program to be loaded, which should be of the \
corresponding file format for the respective plugin. For shell plugins (like \
Waves), use --display-info to get a list of sub-plugin ID's and then use a colon \
to indicate which plugin to load. Examples:\n\n\
\t--plugin LFX-1310\n\
\t--plugin 'AutoTune,KayneWest.fxp;Compressor,SoftKnee.fxp;Limiter'\n\
\t--plugin 'WavesShell-VST' --display-info (list shell sub-plugins)\n\
\t--plugin 'WavesShell-VST:IDFX' (load a shell plugins)",
                          HAS_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeRequired));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_PLUGIN_ROOT,
                          "plugin-root",
                          "Custom non-system directory to use when searching for plugins. Will be searched \
  before system directories if given.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeRequired));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_QUIET,
                          "quiet",
                          "Only log critical errors.",
                          HAS_SHORT_FORM,
                          kProgramOptionTypeEmpty,
                          kProgramOptionArgumentTypeNone));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_REALTIME,
                          "realtime",
                          "Simulate running in realtime by sleeping for any remaining time needed to process \
the given block. Some plugins which are unable to do offline rendering may require this \
option in order to function properly.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeEmpty,
                          kProgramOptionArgumentTypeNone));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_SAMPLE_RATE,
                          "sample-rate",
                          "Sample rate to use when processing. If the input source specifies its own \
sample rate, that value will override the one set by this option. No error checking \
is done for sample rates (other than requiring it to be greater than 0), however \
using unusual sample rates will probably result in weird behavior from plugins.",
                          HAS_SHORT_FORM,
                          kProgramOptionTypeNumber,
                          kProgramOptionArgumentTypeRequired));
    programOptionsSetNumber(options, OPTION_SAMPLE_RATE, (const float)getSampleRate());

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_TEMPO,
                          "tempo",
                          "Tempo to use when processing.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeNumber,
                          kProgramOptionArgumentTypeRequired));
    programOptionsSetNumber(options, OPTION_TEMPO, (float)getTempo());

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_TIME_SIGNATURE,
                          "time-signature",
                          "Set the global time signature. Should be a string formatted like \"3/4\".",
                          NO_SHORT_FORM,
                          kProgramOptionTypeString,
                          kProgramOptionArgumentTypeRequired));
    // This is kind of cheating, because the default time signature could be
    // anything. Realistically we know it will always be hardcoded to 4/4, so this
    // hardcoded string is also relatively safe.
    programOptionsSetCString(options, OPTION_TIME_SIGNATURE, "4/4");

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_VERBOSE,
                          "verbose",
                          "Verbose logging. Logging output is printed in the following form:\n\
(Level) (Frames processed) (Elapsed time in ms) (Logging message)",
                          HAS_SHORT_FORM,
                          kProgramOptionTypeEmpty,
                          kProgramOptionArgumentTypeNone));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_VERSION,
                          "version",
                          "Print full program version and copyright information.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeEmpty,
                          kProgramOptionArgumentTypeNone));

    programOptionsAdd(options, newProgramOptionWithName(
                          OPTION_ZEBRA_SIZE,
                          "zebra-size",
                          "Alternate logging output colors every <argument> frames.",
                          NO_SHORT_FORM,
                          kProgramOptionTypeNumber,
                          kProgramOptionArgumentTypeRequired));
    programOptionsSetNumber(options, OPTION_ZEBRA_SIZE, (const float)getSampleRate());

    return options;
}

void printMrsWatsonQuickstart(const char *argvName)
{
    File argvFile = newFileWithPathCString(argvName);
    const char *programBasename = argvFile->absolutePath->data;
    printf("Run with '--help full' to see extended help for all options.\n");
    printf("Quickstart for effects: %s -p <plugin> -i <input file> -o <output>\n", programBasename);
    printf("Quickstart for instruments: %s -p <name> -m <midi file> -o <output>\n", programBasename);
    printf("\n");
    freeFile(argvFile);
}
