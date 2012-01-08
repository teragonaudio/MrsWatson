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
