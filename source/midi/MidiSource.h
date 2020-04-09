//
// MidiSource.h - MrsWatson
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

#ifndef MrsWatson_MidiSource_h
#define MrsWatson_MidiSource_h

#include "base/CharString.h"
#include "base/Types.h"
#include "midi/MidiSequence.h"

typedef enum {
  MIDI_SOURCE_TYPE_INVALID,
  MIDI_SOURCE_TYPE_FILE,
  NUM_MIDI_SOURCE_TYPES
} MidiSourceType;

typedef boolByte (*OpenMidiSourceFunc)(void *);
typedef boolByte (*ReadMidiEventsFunc)(void *, MidiSequence *,
                                       const unsigned short);
typedef void (*FreeMidiSourceDataFunc)(void *);

typedef struct {
  MidiSourceType midiSourceType;
  CharString sourceName;

  OpenMidiSourceFunc openMidiSource;
  ReadMidiEventsFunc readMidiEvents;
  FreeMidiSourceDataFunc freeMidiSourceData;

  void *extraData;
} MidiSourceMembers;

/**
 * A class which acts as a source for MIDI data.
 */
typedef MidiSourceMembers *MidiSource;

/**
 * Factory method to create a new MIDI source
 * @param midiSourceType Source type, which should be the result of a call to
 * guessMidiSourceType()
 * @param midiSourceName MIDI source name
 * @return MidiSource object, or NULL if no object could be created
 */
MidiSource newMidiSource(MidiSourceType midiSourceType,
                         const CharString midiSourceName);

/**
 * Determine an appropriate source type based on a file name.
 * @param midiSourceTypeString Source name
 * @return Source type
 */
MidiSourceType guessMidiSourceType(const CharString midiSourceTypeString);

/**
 * Free a MidiSource and its associated resources
 * @param self
 */
void freeMidiSource(MidiSource self);

#endif
