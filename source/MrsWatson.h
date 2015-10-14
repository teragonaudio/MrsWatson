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

#ifndef MrsWatson_MrsWatson_h
#define MrsWatson_MrsWatson_h

#include "app/ProgramOption.h"
#include "app/ReturnCodes.h"
#include "base/CharString.h"
#include "io/SampleSource.h"
#include "logging/ErrorReporter.h"
#include "midi/MidiSource.h"
#include "plugin/PluginChain.h"
#include "time/AudioClock.h"

#ifndef EXPORT
#if WINDOWS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#endif

/**
 * Wrapper structure for a MIDI message to be delivered to the plugin.
 */
typedef struct {
    unsigned char *bytes;
    size_t numBytes;
    unsigned long deltaFrames;
} MidiMessage;
typedef MidiMessage *MidiMessages;

typedef struct {
    ErrorReporter errorReporter;
    ProgramOptions options;
    AudioClock audioClock;
    PluginChain pluginChain;
    // Probably a bunch of other stuff, too...
} MrsWatsonMembers;
typedef MrsWatsonMembers *MrsWatson;

/**
 * Legacy main() method. Deprecated.
 */
int mrsWatsonMain(ErrorReporter errorReporter, int argc, char **argv);

/**
 * Create a new empty MrsWatson instance. Do not manually free the returned instance pointer, or you
 * will leak memory. Instead use freeMrsWatson(MrsWatson).
 *
 * @return Initialized MrsWatson instance, or NULL if an allocation failed.
 */
EXPORT
MrsWatson newMrsWatson(void);

/**
 * Initialize a MrsWatson instance. If called multiple times, the instance will be re-initialized.
 *
 * @param self (may not be NULL).
 * @param programOptions Initialized program options to use. The MrsWatson instance takes ownership
 *                       of this data during the call, you must NOT free it afterwards.
 *
 * @return 0 on success, 1 on failure
 */
EXPORT
int mrsWatsonInitialize(MrsWatson self, ProgramOptions programOptions);

/**
 * Initialize a MrsWatson instance. If called multiple times, the instance will be re-initialized.
 * This call is a convenience wrapper for mrsWatsonInitialize(MrsWatson, ProgramOptions).
 *
 * @param self (may not be NULL).
 * @param argc Number of arguments.
 * @param argv Array of arguments. You may free this memory after the call finishes.
 *
 * @return 0 on success, 1 on failure
 */
EXPORT
int mrsWatsonInitializeWithCommandLineArgs(MrsWatson self, int argc, char **argv);

/**
 * Initialize a MrsWatson instance. If called multiple times, the instance
 * will be re-initialized. This call is a convenience wrapper for
 * mrsWatsonInitialize(MrsWatson, ProgramOptions).
 *
 * @param self (may not be NULL).
 * @param serializedOptions Serialized options string. See the help for the `--configuration` option
 *                          for more details on the formatting of this string. If the string cannot
 *                          be parsed to a ProgramOptions object, then this call will fail and the
 *                          MrsWatson instance will remain uninitialized. You may free this memory
 *                          after the call finishes.
 *
 * @return 0 on success, 1 on failure
 */
EXPORT
int mrsWatsonInitializeWithSerializedOptions(MrsWatson self, const char *serializedOptions);

/**
 * Process audio and/or MIDI with a MrsWatson instance.
 *
 * @param self (may not be NULL).
 * @param inputSource Initialized input source (see sampleSourceFactory(const CharString)).
 *                    May be NULL if no input sample source is to be used needed. You may free this
 *                    memory after the call finishes.
 * @param midiSource Initialized MIDI source (see newMidiSource(MidiSourceType, const CharString)).
 *                   May be NULL if no MIDI source is to be used needed. You may free this memory
 *                   after the call finishes.
 * @param inputSource Initialized output source (see sampleSourceFactory(const CharString)). Must
 *                    not be NULL. You may free this memory after the call finishes.
 *
 * @return 0 on success, 1 on failure
 */
EXPORT
int mrsWatsonProcess(MrsWatson self,
                     SampleSource inputSource,
                     MidiSource midiSource,
                     SampleSource outputSource);

/**
 * Process a single audio block (with optional MIDI) with a MrsWatson instance.
 *
 * @param self (may not be NULL).
 * @param inputSamples Array of samples to use for the processing input. May be NULL if not needed.
 *                     You may free this memory after the call finishes.
 * @param midiMessages Array of MIDI events to send to the first plugin in the chain. May be NULL if
 *                     not needed. You may free this memory after the call finishes.
 * @param outputSamples Array of samples to use for the processed output Must not be NULL. You may
 *                      free this memory after the call finishes.
 * @param numChannels Number of channels for both the input and output sample arrays.
 * @param numSamples Number of samples for any given input/output channel index.
 *
 * @return 0 on success, 1 on failure
 */
EXPORT
int mrsWatsonProcessBlock(MrsWatson self,
                          double **inputSamples,
                          MidiMessages midiMessages,
                          double **outputSamples,
                          unsigned short numChannels,
                          unsigned long numSamples);

/**
 * Process audio and/or MIDI with a MrsWatson instance.
 *
 * @param self (may not be NULL).
 * @param inputFileName Input file to read sample data from. May be NULL if not needed. You may free
 *                      this memory after the call finishes.
 * @param midiFileName Input MIDI file to use during processing. May be NULL if not needed. You may
 *                     free this memory after the call finishes.
 * @param outputFileName Output file to write data to. Must not be NULL. You may free this memory
 *                       after the call finishes.
 *
 * @return 0 on success, 1 on failure
 */
EXPORT
int mrsWatsonProcessWithFiles(MrsWatson self,
                              const char *inputFileName,
                              const char *midiFileName,
                              const char *outputFileName);

/**
 * Reset a MrsWatson instance to the default state.
 *
 * @param self (may not be NULL).
 */
EXPORT
void mrsWatsonReset(MrsWatson self);

/**
 * Free a MrsWatson instance and its contents. Safe to call with NULL.
 *
 * @param self (may be NULL).
 */
EXPORT
void freeMrsWatson(MrsWatson self);

#endif
