//
//  AudioClock.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#ifndef MrsWatson_AudioClock_h
#define MrsWatson_AudioClock_h

typedef struct {
  unsigned long currentSample;
} AudioClockMembers;

typedef AudioClockMembers* AudioClock;
extern AudioClock audioClockInstance;

void initAudioClock(void);

void advanceAudioClock(const int blocksize);
unsigned long getAudioClockCurrentSample(void);

#endif
