//
//  MidiSource.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "Types.h"
#include "MidiSequence.h"

#ifndef MrsWatson_MidiSource_h
#define MrsWatson_MidiSource_h

typedef enum {
  MIDI_SOURCE_TYPE_INVALID,
  MIDI_SOURCE_TYPE_FILE,
  NUM_MIDI_SOURCE_TYPES
} MidiSourceType;

typedef boolean (*OpenMidiSourceFunc)(void*);
typedef boolean (*ReadMidiEventsFunc)(void*, MidiSequence);
typedef boolean (*FreeMidiSourceDataFunc)(void*);

#endif
