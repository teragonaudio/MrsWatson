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

#define DEFAULT_SAMPLERATE 44100.0f
#define DEFAULT_NUM_CHANNELS 2
#define DEFAULT_BLOCKSIZE 512
#define DEFAULT_TEMPO 120.0f;
#define DEFAULT_TIMESIG_BEATS_PER_MEASURE 4
#define DEFAULT_TIMESIG_NOTE_VALUE 4

typedef struct {
  float sampleRate;
  int numChannels;
  int blocksize;
  float tempo;
  short timeSignatureBeatsPerMeasure;
  short timeSignatureNoteValue;
} AudioSettingsMembers;

typedef AudioSettingsMembers* AudioSettings;
extern AudioSettings audioSettingsInstance;

void initAudioSettings(void);

float getSampleRate(void);
int getNumChannels(void);
int getBlocksize(void);
float getTempo(void);
short getTimeSignatureBeatsPerMeasure(void);
short getTimeSignatureNoteValue(void);

void setSampleRate(const float sampleRate);
void setNumChannels(const int numChannels);
void setBlocksize(const int blocksize);
void setTempo(const float tempo);
void setTimeSignatureBeatsPerMeasure(const short beatsPerMeasure);
void setTimeSignatureNoteValue(const short noteValue);

#endif
