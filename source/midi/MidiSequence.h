//
// MidiSequence.h - MrsWatson
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

#ifndef MrsWatson_MidiSequence_h
#define MrsWatson_MidiSequence_h

#include "base/LinkedList.h"
#include "midi/MidiEvent.h"


typedef struct {
  LinkedList midiEvents;
  LinkedListIterator _lastEvent;
  int _lastTimestamp;
  int numMidiEventsProcessed;
} MidiSequenceMembers;

/**
 * The purpose of this class is to hold a series of MIDI events in sequential
 * order. After being read from a MidiSource, such as a file or perhaps an
 * actual device, the events are stored here where they can easily be read block
 * by block.
 */
typedef MidiSequenceMembers *MidiSequence;

/**
 * Creating a new MidiSequence object
 * @return MidiSequence instance
 */
MidiSequence newMidiSequence(void);

/**
 * Add an event to the end of the sequence. The event's timestamp must be
 * properly set before making this call. Events added into the sequence in this
 * call are not sorted, it is the responsibility of the caller to add the events
 * sequentially in the order which they should be played back.
 * @param self
 * @param midiEvent MidiEvent to add
 */
void appendMidiEventToSequence(MidiSequence self, MidiEvent midiEvent);

/**
 * Populate a linked list with MIDI events for a given block. This method does
 * not return a linked list in order to optimize for memory usage.
 * @param self
 * @param startTimestamp Sample frame that marks the starting point of the block
 * @param blocksize Blocksize, which determines the range of events that will be
 * added to the list
 * @param outMidiEvents Lists to upend events to
 * @return True if more events remain in the list after this call is complete,
 * false otherwise. This is so that the caller can tell when the end of the MIDI
 * sequence has been reached.
 */
boolByte fillMidiEventsFromRange(MidiSequence self,
                                 const unsigned long startTimestamp,
                                 const unsigned long blocksize,
                                 LinkedList outMidiEvents);

/**
 * Free a MIDI sequence and its associated resources
 * @param self
 */
void freeMidiSequence(MidiSequence midiSequence);

#endif
