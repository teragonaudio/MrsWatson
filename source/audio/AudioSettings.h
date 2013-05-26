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
 * plug-in each time process is called. Note that the blocksize is the number of
 * *frames* sent to the plug-in, so if the channel count is 2 and the blocksize
 * is 512, 1024 samples will be sent to the plug-in. However in that case this
 * function would still return 512.
 * @return Blocksize, in sample frames
 */
unsigned long getBlocksize(void);


double getTempo(void);
short getTimeSignatureBeatsPerMeasure(void);
short getTimeSignatureNoteValue(void);

void setSampleRate(const double sampleRate);
void setNumChannels(const unsigned int numChannels);
void setBlocksize(const unsigned long blocksize);
void setTempo(const double tempo);
void setTempoFromMidiBytes(const byte* bytes);
void setTimeSignatureBeatsPerMeasure(const short beatsPerMeasure);
void setTimeSignatureNoteValue(const short noteValue);
void setTimeSignatureFromMidiBytes(const byte* bytes);

void freeAudioSettings(void);

#endif
