//
//  AudioClock.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "AudioClock.h"

AudioClock audioClockInstance = NULL;

void initAudioClock(void) {
  audioClockInstance = malloc(sizeof(AudioClockMembers));
  audioClockInstance->currentSample = 0;
}

static AudioClock _getAudioClockInstance(void) {
  return audioClockInstance;
}

void advanceAudioClock(const int blocksize) {
  AudioClock audioClock = _getAudioClockInstance();
  audioClock->currentSample += blocksize;
}

unsigned long getAudioClockCurrentSample(void) {
  return _getAudioClockInstance()->currentSample;
}
