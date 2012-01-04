//
//  AudioSettings.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/4/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#import <stdlib.h>
#import "AudioSettings.h"

AudioSettings audioSettingsInstance = NULL;

void initAudioSettings(void) {
  // TODO: This is never freed, but it lives for the life of the program. Do we need to worry about that?
  audioSettingsInstance = malloc(sizeof(AudioSettingsMembers));
  audioSettingsInstance->sampleRate = DEFAULT_SAMPLERATE;
  audioSettingsInstance->numChannels = DEFAULT_NUM_CHANNELS;
  audioSettingsInstance->blocksize = DEFAULT_BLOCKSIZE;
}

AudioSettings getAudioSettings(void) {
  return audioSettingsInstance;
}

void setSampleRate(const float sampleRate) {
  AudioSettings audioSettings = getAudioSettings();
  audioSettings->sampleRate = sampleRate;
}

void setNumChannels(const int numChannels) {
  AudioSettings audioSettings = getAudioSettings();
  audioSettings->numChannels = numChannels;
}

void setBlocksize(const int blocksize) {
  AudioSettings audioSettings = getAudioSettings();
  audioSettings->blocksize = blocksize;
}
