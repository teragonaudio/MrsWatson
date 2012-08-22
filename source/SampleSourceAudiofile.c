//
// SampleSourceAudiofile.c - MrsWatson
// Created by Nik Reiman on 1/22/12.
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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "AudioSettings.h"
#include "SampleSourceAudiofile.h"
#include "EventLogger.h"
#include "SampleSourcePcm.h"

#if USE_LIBAUDIOFILE

boolByte readBlockFromAudiofile(void* sampleSourcePtr, SampleBuffer sampleBuffer) {
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)(sampleSource->extraData);

  int numFramesRead;
  int currentInterlacedSample = 0;
  int currentDeinterlacedSample = 0;
  int currentChannel;
  
  if(extraData->interlacedBuffer == NULL) {
    extraData->interlacedBuffer = (float*)malloc(sizeof(float) * getNumChannels() * getBlocksize());
  }
  memset(extraData->interlacedBuffer, 0, sizeof(float) * getNumChannels() * getBlocksize());

  numFramesRead = afReadFrames(extraData->fileHandle, AF_DEFAULT_TRACK, extraData->interlacedBuffer, getBlocksize());
  // Loop over the number of frames wanted, not the number we actually got. This means that the last block will
  // be partial, but then we write empty data to the end, since the interlaced buffer gets cleared above.
  while(currentInterlacedSample < getBlocksize() * getNumChannels()) {
    for(currentChannel = 0; currentChannel < sampleBuffer->numChannels; currentChannel++) {
      sampleBuffer->samples[currentChannel][currentDeinterlacedSample] = extraData->interlacedBuffer[currentInterlacedSample++];
    }
    currentDeinterlacedSample++;
  }

  sampleSource->numFramesProcessed += numFramesRead;
  if(numFramesRead == 0) {
    logDebug("End of audio file reached");
    return false;
  }
  else if(numFramesRead < 0) {
    logError("Error reading audio file");
    return false;
  }
  else {
    return true;
  }
}

boolByte writeBlockFromAudiofile(void* sampleSourcePtr, const SampleBuffer sampleBuffer) {
  SampleSource sampleSource = (SampleSource)sampleSourcePtr;
  SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)(sampleSource->extraData);
  int result = 0;

  if(extraData->pcmBuffer == NULL) {
    extraData->pcmBuffer = (short*)malloc(sizeof(short) * getNumChannels() * getBlocksize());
  }
  memset(extraData->pcmBuffer, 0, sizeof(short) * getNumChannels() * getBlocksize());
  convertSampleBufferToPcmData(sampleBuffer, extraData->pcmBuffer);

  result = afWriteFrames(extraData->fileHandle, AF_DEFAULT_TRACK, extraData->pcmBuffer, getBlocksize());
  sampleSource->numFramesProcessed += getBlocksize() * getNumChannels();
  return (result == 1);
}

void freeSampleSourceDataAudiofile(void* sampleSourceDataPtr) {
  SampleSourceAudiofileData extraData = (SampleSourceAudiofileData)sampleSourceDataPtr;
  if(extraData->fileHandle != NULL) {
    afCloseFile(extraData->fileHandle);
  }
  if(extraData->interlacedBuffer != NULL) {
    free(extraData->interlacedBuffer);
  }
  if(extraData->pcmBuffer != NULL) {
    free(extraData->pcmBuffer);
  }
  free(extraData);
}

#endif
