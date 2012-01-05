//
//  MidiEvent.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "MidiEvent.h"

MidiEvent newMidiEvent(void) {
  MidiEvent midiEvent = malloc(sizeof(MidiEventMembers));

  midiEvent->eventType = MIDI_TYPE_INVALID;
  midiEvent->status = 0;
  midiEvent->data1 = 0;
  midiEvent->data2 = 0;
  midiEvent->sysex = NULL;

  return midiEvent;
}

void freeMidiEvent(MidiEvent midiEvent) {
  if(midiEvent->eventType == MIDI_TYPE_SYSEX) {
    free(midiEvent->sysex);
  }
  free(midiEvent);
}
