//
//  SampleSourceSilence.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "SampleSourceSilence.h"
#include "AudioSettings.h"

static boolean _openSampleSourceSilence(void* sampleSourcePtr, const SampleSourceOpenAs openAs) {
  SampleSource sampleSource = sampleSourcePtr;
  sampleSource->openedAs = openAs;
  return true;
}

static boolean _readBlockFromSilence(void* sampleSourcePtr, SampleBuffer sampleBuffer) {
  clearSampleBuffer(sampleBuffer);
  return true;
}

static boolean _writeBlockFromSilence(void* sampleSourcePtr, const SampleBuffer sampleBuffer) {
  return true;
}

static void _freeInputSourceDataSilence(void* sampleSourceDataPtr) {
}

SampleSource newSampleSourceSilence(const CharString sampleSourceName) {
  SampleSource sampleSource = malloc(sizeof(SampleSourceMembers));

  sampleSource->sampleSourceType = SAMPLE_SOURCE_TYPE_SILENCE;
  sampleSource->openedAs = SAMPLE_SOURCE_OPEN_NOT_OPENED;
  sampleSource->sourceName = newCharString();
  copyCharStrings(sampleSource->sourceName, sampleSourceName);
  sampleSource->numChannels = getNumChannels();
  sampleSource->sampleRate = getSampleRate();

  sampleSource->openSampleSource = _openSampleSourceSilence;
  sampleSource->readSampleBlock = _readBlockFromSilence;
  sampleSource->writeSampleBlock = _writeBlockFromSilence;
  sampleSource->freeSampleSourceData = _freeInputSourceDataSilence;

  return sampleSource;
}
