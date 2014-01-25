//
// AudioSettings.h - MrsWatson
// Created by Nik Reiman on 1/4/12.
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

#ifndef MrsWatson_AudioSettings_h
#define MrsWatson_AudioSettings_h

#include "base/Types.h"
#include "base/CharString.h"

#define DEFAULT_SAMPLE_RATE 44100.0f
#define DEFAULT_NUM_CHANNELS 2
#define DEFAULT_BLOCKSIZE 512l
#define DEFAULT_TIME_DIVISION 96
#define DEFAULT_BITRATE 16
#define DEFAULT_TEMPO 120.0f
#define DEFAULT_TIMESIG_BEATS_PER_MEASURE 4
#define DEFAULT_TIMESIG_NOTE_VALUE 4

typedef struct {
  double sampleRate;
  unsigned int numChannels;
  unsigned long blocksize;
  double tempo;
  short timeSignatureBeatsPerMeasure;
  short timeSignatureNoteValue;
} AudioSettingsMembers;

typedef AudioSettingsMembers* AudioSettings;
extern AudioSettings audioSettingsInstance;

/**
 * Initialize the global audio settings instance. Since many different classes
 * require quick access to the audio settings, this is one of the few classes
 * that has a global singleton instance rather than a "new" allocator.
 */
void initAudioSettings(void);

/**
 * Get the current sample rate.
 * @return Sample rate in Hertz
 */
double getSampleRate(void);

/**
 * Get the number of output channels.
 * @return Number of channels
 */
unsigned int getNumChannels(void);

/**
 * Give the current block size, which is the number of sample frames sent to the
 * plugin each time process is called. Note that the blocksize is the number of
 * *frames* sent to the plugin, so if the channel count is 2 and the blocksize
 * is 512, 1024 samples will be sent to the plugin. However in that case this
 * function would still return 512.
 * @return Blocksize, in sample frames
 */
unsigned long getBlocksize(void);

/**
 * Get the current tempo, in beats per minute
 * @return Temo in BPM
 */
double getTempo(void);

/**
 * Get the current time signature's numerator, the number of beats per measure.
 * @return Time signature numerator
 */
short getTimeSignatureBeatsPerMeasure(void);

/**
 * Get the current time signatures denominator, the value of one beat unit.
 * @return Time signature denominator
 */
short getTimeSignatureNoteValue(void);

/**
 * Set the sample rate to be used during processing. This must be set before the
 * plugin chain is initialized. This function only requires a nonzero value,
 * however some plugins may behave strangely when sent unusual sample rates.
 * @param sampleRate Sample rate, in Hertz
 */
void setSampleRate(const double sampleRate);

/**
 * Set the number of channels to be used during processing. Note that if the
 * input source defines a channel called, it may override this value.
 * @param numChannels Number of channels
 */
void setNumChannels(const unsigned int numChannels);

/**
 * Set the blocksize to be used during processing. Again this should be called
 * before initializing the plugin chain.
 * @param blocksize Blocksize in sample frames
 */
void setBlocksize(const unsigned long blocksize);

/**
 * Set tempo to be used during processing.
 * @param tempo Tempo in beats per minute
 */
void setTempo(const double tempo);

/**
 * MIDI files represent tempo in meta events with a three-byte payload. This
 * method transforms the three byte sequence from such file into an actual tempo
 * in beats per minute, and then sets the global tempo to this value.
 * @param bytes Three byte sequence as read from a MIDI file
 */
void setTempoFromMidiBytes(const byte* bytes);

/**
 * Set the time signature's numerator. This function does very little error
 * checking, but it does require a non-zero value. However, many plugins may act
 * strangely with unusual time signatures.
 * @param beatsPerMeasure Time signature numerator
 * @return True if successfully set, false otherwise
 */
boolByte setTimeSignatureBeatsPerMeasure(const short beatsPerMeasure);

/**
 * Set the time signature's denominator. This function does very little error
 * checking, but it does require a non-zero value. However, many plugins may act
 * strangely with unusual time signatures.
 * @param noteValue Time signature denominator
 * @return True if successfully set, false otherwise
 */
boolByte setTimeSignatureNoteValue(const short noteValue);

/**
 * MIDI files represent musical time signature with a two-byte sequence. This
 * function takes two bytes, derives the corresponding time signature, and sets
 * it in the global instance.
 * @param bytes Two byte sequence as read from a MIDI file
 * @return True if successfully set, false otherwise
 */
boolByte setTimeSignatureFromMidiBytes(const byte* bytes);

/**
 * Set the time signature from a string, should look like "3/4".
 * @param signature Time signature to set
 * @return True if successfully set, false otherwise
 */
boolByte setTimeSignatureFromString(const CharString signature);

/**
 * Release memory of the global audio settings instance. Any attempt to use the
 * audio settings functions after this has been called will result in undefined
 * behavior.
 */
void freeAudioSettings(void);

#endif
