//
//  MidiSequence.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "MidiSequence.h"
#include "MidiEvent.h"

MidiSequence newMidiSequence(void) {
  MidiSequence midiSequence = malloc(sizeof(MidiSequenceMembers));

  midiSequence->midiEvents = newLinkedList();

  return midiSequence;
}

void freeMidiSequence(MidiSequence midiSequence) {
  freeLinkedListAndItems(midiSequence->midiEvents, (LinkedListFreeItemFunc)freeMidiEvent);
  free(midiSequence);
}
