//
//  MidiSource.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "Types.h"
#include "MidiSequence.h"
#include "CharString.h"

#ifndef MrsWatson_MidiSource_h
#define MrsWatson_MidiSource_h

typedef enum {
  MIDI_SOURCE_TYPE_INVALID,
  MIDI_SOURCE_TYPE_FILE,
  NUM_MIDI_SOURCE_TYPES
} MidiSourceType;

typedef boolean (*OpenMidiSourceFunc)(void*);
typedef boolean (*ReadMidiEventsFunc)(void*, MidiSequence);
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
