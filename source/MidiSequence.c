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

MidiSequence newMidiSequence(void) {
  MidiSequence midiSequence = malloc(sizeof(MidiSequenceMembers));

  midiSequence->midiEvents = newLinkedList();

  return midiSequence;
}

void appendMidiEventToSequence(MidiSequence midiSequence, MidiEvent midiEvent) {
  appendItemToList(midiSequence->midiEvents, midiEvent);
}

boolean fillMidiEventsFromRange(MidiSequence midiSequence, const unsigned long startTimestamp, const int blocksize, LinkedList outMidiEvents) {
  LinkedListIterator iterator = midiSequence->midiEvents;
  const unsigned long stopTimestamp = startTimestamp + blocksize;
  boolean result = true;

  // TODO: This is inefficient. Maybe convert the sequence to an array before running?
  while(true) {
    MidiEvent midiEvent = iterator->item;
    if(stopTimestamp < midiEvent->timestamp) {
      // We have not yet reached this event, continue iterating
    }
    else if(startTimestamp <= midiEvent->timestamp && stopTimestamp > midiEvent->timestamp) {
      midiEvent->deltaFrames = midiEvent->timestamp - startTimestamp;
      appendItemToList(outMidiEvents, midiEvent);
    }
    else if(startTimestamp > midiEvent->timestamp) {
      // We have aready passed this event, continue iterating
    }

    // Last item in the list
    if(iterator->nextItem == NULL) {
      if(startTimestamp <= midiEvent->timestamp && stopTimestamp > midiEvent->timestamp) {
        result = false;
      }
      break;
    }
    iterator = iterator->nextItem;
  }

  return result;
}

void freeMidiSequence(MidiSequence midiSequence) {
  freeLinkedListAndItems(midiSequence->midiEvents, (LinkedListFreeItemFunc)freeMidiEvent);
  free(midiSequence);
}
