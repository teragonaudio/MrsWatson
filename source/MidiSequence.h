//
//  MidiSequence.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "LinkedList.h"
#include "MidiEvent.h"

#ifndef MrsWatson_MidiSequence_h
#define MrsWatson_MidiSequence_h

typedef struct {
  LinkedList midiEvents;
} MidiSequenceMembers;

typedef MidiSequenceMembers* MidiSequence;

MidiSequence newMidiSequence(void);

void appendMidiEventToSequence(MidiSequence midiSequence, MidiEvent midiEvent);
boolean fillMidiEventsFromRange(MidiSequence midiSequence, const unsigned long startTimestamp, const int blocksize, LinkedList outMidiEvents);

void freeMidiSequence(MidiSequence midiSequence);

#endif
