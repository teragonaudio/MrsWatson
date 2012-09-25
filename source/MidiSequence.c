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
#include "EventLogger.h"

MidiSequence newMidiSequence(void) {
  MidiSequence midiSequence = malloc(sizeof(MidiSequenceMembers));

  midiSequence->midiEvents = newLinkedList();
  midiSequence->_lastEvent = midiSequence->midiEvents;
  midiSequence->_lastTimestamp = 0;
  midiSequence->numMidiEventsProcessed = 0;

  return midiSequence;
}

void appendMidiEventToSequence(MidiSequence midiSequence, MidiEvent midiEvent) {
  if(midiSequence != NULL && midiEvent != NULL) {
    appendItemToList(midiSequence->midiEvents, midiEvent);
  }
}

boolByte fillMidiEventsFromRange(MidiSequence midiSequence, const unsigned long startTimestamp, const int blocksize, LinkedList outMidiEvents) {
  MidiEvent midiEvent;
  LinkedListIterator iterator = midiSequence->_lastEvent;
  const unsigned long stopTimestamp = startTimestamp + blocksize;

  while(true) {
    if((iterator == NULL) || (iterator->item == NULL)) {
      return false;
    }

    midiEvent = iterator->item;
    if(stopTimestamp < midiEvent->timestamp) {
      // We have not yet reached this event, stop iterating
      break;
    }
    else if(startTimestamp <= midiEvent->timestamp && stopTimestamp > midiEvent->timestamp) {
      midiEvent->deltaFrames = midiEvent->timestamp - startTimestamp;
      appendItemToList(outMidiEvents, midiEvent);
      midiSequence->_lastEvent = iterator->nextItem;
      midiSequence->numMidiEventsProcessed++;
    }
    else if(startTimestamp > midiEvent->timestamp) {
      logInternalError("Inconsistent MIDI sequence ordering");
    }

    // Last item in the list
    if(iterator->nextItem == NULL) {
      if(startTimestamp <= midiEvent->timestamp && stopTimestamp > midiEvent->timestamp) {
        return false;
      }
      break;
    }
    iterator = iterator->nextItem;
  }

  return true;
}

void freeMidiSequence(MidiSequence midiSequence) {
  freeLinkedListAndItems(midiSequence->midiEvents, (LinkedListFreeItemFunc)freeMidiEvent);
  free(midiSequence);
}
