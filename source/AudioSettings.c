//
// AudioSettings.c - MrsWatson
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

#include <stdio.h>
#include <stdlib.h>
#include "AudioSettings.h"
#include "EventLogger.h"

AudioSettings audioSettingsInstance = NULL;

void initAudioSettings(void) {
  // TODO: This is never freed, but it lives for the life of the program. Do we need to worry about that?
  audioSettingsInstance = malloc(sizeof(AudioSettingsMembers));
  audioSettingsInstance->sampleRate = DEFAULT_SAMPLE_RATE;
  audioSettingsInstance->numChannels = DEFAULT_NUM_CHANNELS;
  audioSettingsInstance->blocksize = DEFAULT_BLOCKSIZE;
  audioSettingsInstance->tempo = DEFAULT_TEMPO;
  audioSettingsInstance->timeSignatureBeatsPerMeasure = DEFAULT_TIMESIG_BEATS_PER_MEASURE;
  audioSettingsInstance->timeSignatureNoteValue = DEFAULT_TIMESIG_NOTE_VALUE;
}

static AudioSettings _getAudioSettings(void) {
  return audioSettingsInstance;
}

double getSampleRate(void) {
  return _getAudioSettings()->sampleRate;
}

int getNumChannels(void) {
  return _getAudioSettings()->numChannels;
}

int getBlocksize(void) {
  return _getAudioSettings()->blocksize;
}

double getTempo(void) {
  return _getAudioSettings()->tempo;
}

short getTimeSignatureBeatsPerMeasure(void) {
  return _getAudioSettings()->timeSignatureBeatsPerMeasure;
}

short getTimeSignatureNoteValue(void) {
  return _getAudioSettings()->timeSignatureNoteValue;
}


void setSampleRate(const double sampleRate) {
  if(sampleRate == 0.0f) {
    logCritical("Ignoring attempt to set sample rate to 0");
    return;
  }
  _getAudioSettings()->sampleRate = sampleRate;
}

void setNumChannels(const int numChannels) {
  if(numChannels == 0) {
    logCritical("Ignoring attempt to set num channels to 0");
    return;
  }
  _getAudioSettings()->numChannels = numChannels;
}

void setBlocksize(const int blocksize) {
  if(blocksize == 0) {
    logCritical("Ignoring attempt to set blocksize to 0");
    return;
  }
  _getAudioSettings()->blocksize = blocksize;
}

void setTempo(const double tempo) {
  if(tempo == 0.0f) {
    logCritical("Ignoring attempt to set tempo to 0");
    return;
  }
  _getAudioSettings()->tempo = tempo;
}

void setTimeSignatureBeatsPerMeasure(const short beatsPerMeasure) {
  // Bit of an easter egg :)
  if(beatsPerMeasure < 2 || beatsPerMeasure > 12) {
    logInfo("Freaky time signature, but whatever you say...");
  }
  if(beatsPerMeasure == 0) {
    logCritical("Ignoring attempt to set time signature numerator to 0");
    return;
  }
  _getAudioSettings()->timeSignatureBeatsPerMeasure = beatsPerMeasure;
}

void setTimeSignatureNoteValue(const short noteValue) {
  // Bit of an easter egg :)
  if(!(noteValue == 2 || noteValue == 4 || noteValue == 8 || noteValue == 16) || noteValue < 2 || noteValue > 16) {
    logInfo("Interesting time signature you've chosen. I'm sure this piece is going to sound great...");
  }
  if(noteValue == 0) {
    logCritical("Ignoring attempt to set time signature denominator to 0");
    return;
  }
  _getAudioSettings()->timeSignatureNoteValue = noteValue;
}

void freeAudioSettings(void) {
  free(audioSettingsInstance);
  audioSettingsInstance = NULL;
}
