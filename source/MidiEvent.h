//
//  MidiEvent.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "Types.h"

#ifndef MrsWatson_MidiEvent_h
#define MrsWatson_MidiEvent_h

typedef enum {
  MIDI_TYPE_INVALID,
  MIDI_TYPE_REGULAR,
  MIDI_TYPE_SYSEX,
  NUM_MIDI_TYPES
} MidiEventType;

typedef struct {
  MidiEventType eventType;
  unsigned long timestamp;
  byte status;
  byte data1;
  byte data2;
  byte* sysex;
} MidiEventMembers;

typedef MidiEventMembers* MidiEvent;

MidiEvent newMidiEvent(void);
void freeMidiEvent(MidiEvent midiEvent);

#endif
