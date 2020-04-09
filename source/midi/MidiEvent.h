//
// MidiEvent.h - MrsWatson
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

#ifndef MrsWatson_MidiEvent_h
#define MrsWatson_MidiEvent_h

#include "base/Types.h"

typedef enum {
  MIDI_TYPE_INVALID,
  MIDI_TYPE_VOICE,
  MIDI_TYPE_SYSTEM,
  MIDI_TYPE_META,
  NUM_MIDI_TYPES
} MidiEventType;

typedef struct {
  MidiEventType eventType;
  unsigned long deltaFrames;
  unsigned long timestamp;
  byte status;
  byte data1;
  byte data2;
  byte *extraData;
  size_t extraDataSize;
} MidiEventMembers;
typedef MidiEventMembers *MidiEvent;

// MIDI Meta Event types
#define MIDI_META_TYPE_TEXT 0x01
#define MIDI_META_TYPE_COPYRIGHT 0x02
#define MIDI_META_TYPE_SEQUENCE_NAME 0x03
#define MIDI_META_TYPE_INSTRUMENT 0x04
#define MIDI_META_TYPE_LYRIC 0x05
#define MIDI_META_TYPE_MARKER 0x06
#define MIDI_META_TYPE_CUE_POINT 0x07
#define MIDI_META_TYPE_PROGRAM_NAME 0x08
#define MIDI_META_TYPE_DEVICE_NAME 0x09
#define MIDI_META_TYPE_TEMPO 0x51
#define MIDI_META_TYPE_TIME_SIGNATURE 0x58
#define MIDI_META_TYPE_KEY_SIGNATURE 0x59
#define MIDI_META_TYPE_PROPRIETARY 0x7f
#define MIDI_META_TYPE_TRACK_END 0x2f

/**
 * Create a new MIDI event
 * @return MidiEvent object
 */
MidiEvent newMidiEvent(void);

/**
 * Free a MIDI event object and its associated resources
 * @param self
 */
void freeMidiEvent(MidiEvent self);

#endif
