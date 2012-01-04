//
//  AudioSettings.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/4/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#ifndef MrsWatson_AudioSettings_h
#define MrsWatson_AudioSettings_h

#define DEFAULT_SAMPLERATE 44100.0f
#define DEFAULT_NUM_CHANNELS 2
#define DEFAULT_BLOCKSIZE 512

typedef struct {
  float sampleRate;
  int numChannels;
  int blocksize;
} AudioSettingsMembers;

typedef AudioSettingsMembers* AudioSettings;
extern AudioSettings audioSettingsInstance;

void initAudioSettings(void);

AudioSettings getAudioSettings(void);
void setSampleRate(const float sampleRate);
void setNumChannels(const int numChannels);
void setBlocksize(const int blocksize);

#endif
