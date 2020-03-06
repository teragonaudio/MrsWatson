//
// AudioSettings.c - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
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

#include "AudioSettings.h"

#include "logging/EventLogger.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AudioSettings audioSettingsInstance = NULL;

void initAudioSettings(void) {
  if (audioSettingsInstance != NULL) {
    freeAudioSettings();
  }

  audioSettingsInstance = malloc(sizeof(AudioSettingsMembers));
  audioSettingsInstance->sampleRate = DEFAULT_SAMPLE_RATE;
  audioSettingsInstance->numChannels = DEFAULT_NUM_CHANNELS;
  audioSettingsInstance->blocksize = DEFAULT_BLOCKSIZE;
  audioSettingsInstance->tempo = DEFAULT_TEMPO;
  audioSettingsInstance->timeSignatureBeatsPerMeasure =
      DEFAULT_TIMESIG_BEATS_PER_MEASURE;
  audioSettingsInstance->timeSignatureNoteValue = DEFAULT_TIMESIG_NOTE_VALUE;
  audioSettingsInstance->bitDepth = kBitDepthDefault;
}

static AudioSettings _getAudioSettings(void) {
  if (audioSettingsInstance == NULL) {
    initAudioSettings();
  }

  return audioSettingsInstance;
}

SampleRate getSampleRate(void) { return _getAudioSettings()->sampleRate; }

ChannelCount getNumChannels(void) { return _getAudioSettings()->numChannels; }

SampleCount getBlocksize(void) { return _getAudioSettings()->blocksize; }

Tempo getTempo(void) { return _getAudioSettings()->tempo; }

unsigned short getTimeSignatureBeatsPerMeasure(void) {
  return _getAudioSettings()->timeSignatureBeatsPerMeasure;
}

unsigned short getTimeSignatureNoteValue(void) {
  return _getAudioSettings()->timeSignatureNoteValue;
}

BitDepth getBitDepth(void) { return _getAudioSettings()->bitDepth; }

boolByte setSampleRate(const SampleRate sampleRate) {
  if (sampleRate <= 0.0f) {
    logError("Can't set sample rate to %f", sampleRate);
    return false;
  }

  logInfo("Setting sample rate to %gHz", sampleRate);
  _getAudioSettings()->sampleRate = sampleRate;
  return true;
}

boolByte setNumChannels(const ChannelCount numChannels) {
  if (numChannels <= 0) {
    logError("Can't set channel count to %d", numChannels);
    return false;
  }

  logInfo("Setting %d channels", numChannels);
  _getAudioSettings()->numChannels = numChannels;
  return true;
}

boolByte setBlocksize(const SampleCount blocksize) {
  if (blocksize <= 0) {
    logError("Can't set invalid blocksize %d", blocksize);
    return false;
  }

  logInfo("Setting blocksize to %ld", blocksize);
  _getAudioSettings()->blocksize = blocksize;
  return true;
}

boolByte setTempo(const Tempo tempo) {
  if (tempo <= 0.0f) {
    logError("Cannot set tempo to %f", tempo);
    return false;
  }

  logInfo("Setting tempo to %f", tempo);
  _getAudioSettings()->tempo = tempo;
  return true;
}

void setTempoFromMidiBytes(const byte *bytes) {
  double tempo;
  unsigned long beatLengthInMicroseconds = 0;

  if (bytes != NULL) {
    beatLengthInMicroseconds = (unsigned long)(0x00000000 | (bytes[0] << 16) |
                                               (bytes[1] << 8) | (bytes[2]));
    // Convert beats / microseconds -> beats / minutes
    tempo = (1000000.0 / (double)beatLengthInMicroseconds) * 60.0;
    setTempo((float)tempo);
  }
}

boolByte setTimeSignatureBeatsPerMeasure(const unsigned short beatsPerMeasure) {
  // Bit of an easter egg :)
  if (beatsPerMeasure < 2 || beatsPerMeasure > 12) {
    logInfo("Freaky time signature, but whatever you say...");
  }

  if (beatsPerMeasure <= 0) {
    logError("Ignoring attempt to set time signature numerator to %d",
             beatsPerMeasure);
    return false;
  }

  _getAudioSettings()->timeSignatureBeatsPerMeasure = beatsPerMeasure;
  return true;
}

boolByte setTimeSignatureNoteValue(const unsigned short noteValue) {
  // Bit of an easter egg :)
  if (!(noteValue == 2 || noteValue == 4 || noteValue == 8 ||
        noteValue == 16) ||
      noteValue < 2 || noteValue > 16) {
    logInfo("Interesting time signature you've chosen. I'm sure this piece is "
            "going to sound great...");
  }

  if (noteValue <= 0) {
    logError("Ignoring attempt to set time signature denominator to %d",
             noteValue);
    return false;
  }

  _getAudioSettings()->timeSignatureNoteValue = noteValue;
  return true;
}

boolByte setTimeSignatureFromString(const CharString signature) {
  char *slash = NULL;
  unsigned short numerator = 0;
  unsigned short denominator = 0;

  if (!charStringIsEmpty(signature)) {
    slash = strchr(signature->data, '/');

    if (slash != NULL) {
      *slash = '\0';
      numerator = (unsigned short)strtod(signature->data, NULL);
      denominator = (unsigned short)strtod(slash + 1, NULL);

      if (numerator > 0 && denominator > 0) {
        return (boolByte)(setTimeSignatureBeatsPerMeasure(numerator) &&
                          setTimeSignatureNoteValue(denominator));
      }
    }
  }

  return false;
}

boolByte setTimeSignatureFromMidiBytes(const byte *bytes) {
  if (bytes != NULL) {
    return (boolByte)(
        setTimeSignatureBeatsPerMeasure(bytes[0]) &&
        setTimeSignatureNoteValue((unsigned const short)powl(2, bytes[1])));
  }

  return false;
}

boolByte setBitDepth(const BitDepth bitDepth) {
  switch (bitDepth) {
  case kBitDepth8Bit:
  case kBitDepth16Bit:
  case kBitDepth24Bit:
  case kBitDepth32Bit:
    _getAudioSettings()->bitDepth = bitDepth;
    return true;

  default:
    logError("Invalid bit depth %d", bitDepth);
    return false;
  }
}

void freeAudioSettings(void) {
  free(audioSettingsInstance);
  audioSettingsInstance = NULL;
}
