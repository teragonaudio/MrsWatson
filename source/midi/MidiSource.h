//
// MidiSource.h - MrsWatson
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

#ifndef MrsWatson_MidiSource_h
#define MrsWatson_MidiSource_h

#include "base/CharString.h"
#include "base/Types.h"
#include "sequencer/MidiSequence.h"

typedef enum {
  MIDI_SOURCE_TYPE_INVALID,
  MIDI_SOURCE_TYPE_FILE,
  NUM_MIDI_SOURCE_TYPES
} MidiSourceType;

typedef boolByte (*OpenMidiSourceFunc)(void*);
typedef boolByte (*ReadMidiEventsFunc)(void*, MidiSequence);
typedef void (*FreeMidiSourceDataFunc)(void*);

typedef struct {
  MidiSourceType midiSourceType;
  CharString sourceName;

  OpenMidiSourceFunc openMidiSource;
  ReadMidiEventsFunc readMidiEvents;
  FreeMidiSourceDataFunc freeMidiSourceData;

  void* extraData;
} MidiSourceMembers;

typedef MidiSourceMembers* MidiSource;

MidiSourceType guessMidiSourceType(const CharString midiSourceTypeString);
MidiSource newMidiSource(MidiSourceType midiSourceType, const CharString midiSourceName);
void freeMidiSource(MidiSource midiSource);

#endif
