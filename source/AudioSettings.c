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
#import "EventLogger.h"

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
  if(sampleRate == 0.0f) {
    logCritical("Ignoring attempt to set sample rate to 0");
    return;
  }
  AudioSettings audioSettings = getAudioSettings();
  audioSettings->sampleRate = sampleRate;
}

void setNumChannels(const int numChannels) {
  if(numChannels == 0) {
    logCritical("Ignoring attempt to set num channels to 0");
    return;
  }
  AudioSettings audioSettings = getAudioSettings();
  audioSettings->numChannels = numChannels;
}

void setBlocksize(const int blocksize) {
  if(blocksize == 0) {
    logCritical("Ignoring attempt to set blocksize to 0");
    return;
  }
  AudioSettings audioSettings = getAudioSettings();
  audioSettings->blocksize = blocksize;
}
