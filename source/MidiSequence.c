//
// MidiSequence.c - MrsWatson
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
