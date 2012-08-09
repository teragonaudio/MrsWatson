//
// AudioClock.c - MrsWatson
// Created by Nik Reiman on 1/5/12.
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
#include "AudioClock.h"

AudioClock audioClockInstance = NULL;

void initAudioClock(void) {
  audioClockInstance = (AudioClock)malloc(sizeof(AudioClockMembers));
  audioClockInstance->currentSample = 0;
}

static AudioClock _getAudioClockInstance(void) {
  return audioClockInstance;
}

void advanceAudioClock(const int blocksize) {
  AudioClock audioClock = _getAudioClockInstance();
  if(audioClock->currentSample == 0) {
    audioClock->transportChanged = true;
    audioClock->isPlaying = true;
  }
  else {
    audioClock->transportChanged = false;
  }
  audioClock->currentSample += blocksize;
}

void stopAudioClock(void) {
  _getAudioClockInstance()->isPlaying = false;
  _getAudioClockInstance()->transportChanged = true;
}

unsigned long getAudioClockCurrentSample(void) {
  return _getAudioClockInstance()->currentSample;
}

boolByte getAudioClockTransportChanged(void) {
  return _getAudioClockInstance()->transportChanged;
}

boolByte getAudioClockIsPlaying(void) {
  return _getAudioClockInstance()->isPlaying;
}

void freeAudioClock(void) {
  free(audioClockInstance);
  audioClockInstance = NULL;
}
