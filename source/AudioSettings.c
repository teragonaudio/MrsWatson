//
//  AudioSettings.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/4/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "AudioSettings.h"
#include "EventLogger.h"

AudioSettings audioSettingsInstance = NULL;

void initAudioSettings(void) {
  // TODO: This is never freed, but it lives for the life of the program. Do we need to worry about that?
  audioSettingsInstance = malloc(sizeof(AudioSettingsMembers));
  audioSettingsInstance->sampleRate = DEFAULT_SAMPLERATE;
  audioSettingsInstance->numChannels = DEFAULT_NUM_CHANNELS;
  audioSettingsInstance->blocksize = DEFAULT_BLOCKSIZE;
  audioSettingsInstance->tempo = DEFAULT_TEMPO;
  audioSettingsInstance->timeSignatureBeatsPerMeasure = DEFAULT_TIMESIG_BEATS_PER_MEASURE;
  audioSettingsInstance->timeSignatureNoteValue = DEFAULT_TIMESIG_NOTE_VALUE;
}

static AudioSettings _getAudioSettings(void) {
  return audioSettingsInstance;
}

float getSampleRate(void) {
  return _getAudioSettings()->sampleRate;
}

int getNumChannels(void) {
  return _getAudioSettings()->numChannels;
}

int getBlocksize(void) {
  return _getAudioSettings()->blocksize;
}

float getTempo(void) {
  return _getAudioSettings()->tempo;
}

short getTimeSignatureBeatsPerMeasure(void) {
  return _getAudioSettings()->timeSignatureBeatsPerMeasure;
}

short getTimeSignatureNoteValue(void) {
  return _getAudioSettings()->timeSignatureNoteValue;
}


void setSampleRate(const float sampleRate) {
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

void setTempo(const float tempo) {
  if(tempo == 0.0f) {
    logCritical("Ignoring attempt to set tempo to 0");
    return;
  }
  _getAudioSettings()->tempo = tempo;
}

void setTimeSignatureBeatsPerMeasure(const short beatsPerMeasure) {
  // TODO: Check for invalid values
  _getAudioSettings()->timeSignatureBeatsPerMeasure = beatsPerMeasure;
}

void setTimeSignatureNoteValue(const short noteValue) {
  // TODO: Check for invalid note values
  _getAudioSettings()->timeSignatureNoteValue = noteValue;
}
