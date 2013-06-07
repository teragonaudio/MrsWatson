//
// AudioClock.h - MrsWatson
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

#ifndef MrsWatson_AudioClock_h
#define MrsWatson_AudioClock_h

#include "base/Types.h"

/**
 * The AudioClock class keeps track of the sequence time and delivers the
 * position in a variety of formats. Unlike most other classes, this one
 * maintains a singleton instance because it must be accessed from C++
 * callbacks where it is difficult to pass a void* pointer.
 */

typedef struct {
  boolByte transportChanged;
  boolByte isPlaying;
  unsigned long currentFrame;
} AudioClockMembers;
typedef AudioClockMembers* AudioClock;
extern AudioClock audioClockInstance;

/**
 * Initialize the global audio clock instance. Should be called fairly
 * early in the program initialization, as other components may depend
 * on knowing the current position.
 */
void initAudioClock(void);

/**
 * Get a reference to the global audio clock instance.
 * @return Reference to global audio clock, or NULL if the global instance has
 * not yet been initialized.
 */
AudioClock getAudioClock(void);

/**
 * Advanced the global audio clock by a given number of samples. This should be
 * called after processing each block.
 * @param self
 * @param blocksize Block size in sample frames.
 */
void advanceAudioClock(AudioClock self, const unsigned long blocksize);

/**
 * Indicate that playback is stopped.
 * @param self
 */
void audioClockStop(AudioClock self);

/**
 * Free an audio clock instance and its associated resources.
 * @param self
 */
void freeAudioClock(AudioClock self);

#endif
