//
//  MidiSourceFile.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "MidiSource.h"
#include "MidiSourceFile.h"
#include "EventLogger.h"

static boolean _openMidiSourceFile(void* midiSourcePtr) {
  MidiSource midiSource = midiSourcePtr;
  MidiSourceFileData extraData = midiSource->extraData;

  extraData->fileHandle = fopen(midiSource->sourceName->data, "rb");
  if(extraData->fileHandle == NULL) {
    logError("MIDI file '%s' could not be opened for reading", midiSource->sourceName->data);
    return false;
  }

  return true;
}

static boolean _readMidiEventsFile(void* midiSourcePtr, MidiSequence midiSequence) {
  for(int i = 0; i < 8; i++) {
    MidiEvent onEvent = newMidiEvent();
    onEvent->eventType = MIDI_TYPE_REGULAR;
    onEvent->timestamp = (unsigned long)(20000 * i);
    onEvent->status = 0x90; // Note on
    onEvent->data1 = 0x45; // A4
    onEvent->data2 = 0x7f; // Maximum velocity
    appendMidiEventToSequence(midiSequence, onEvent);

    MidiEvent offEvent = newMidiEvent();
    offEvent->eventType = MIDI_TYPE_REGULAR;
    offEvent->timestamp = (unsigned long)(200000 * i) + 10000;
    offEvent->status = 0x90; // Note on (0 velocity for off)
    offEvent->data1 = 0x45; // A4
    offEvent->data2 = 0x00;
    appendMidiEventToSequence(midiSequence, offEvent);
  }

  return true;
}

static void _freeMidiEventsFile(void *midiSourceDataPtr) {
  MidiSourceFileData extraData = midiSourceDataPtr;
  if(extraData->fileHandle != NULL) {
    fclose(extraData->fileHandle);
  }
  free(extraData);
}

MidiSource newMidiSourceFile(const CharString midiSourceName) {
  MidiSource midiSource = malloc(sizeof(MidiSourceMembers));

  midiSource->midiSourceType = MIDI_SOURCE_TYPE_FILE;
  midiSource->sourceName = newCharString();
  copyCharStrings(midiSource->sourceName, midiSourceName);

  midiSource->openMidiSource = _openMidiSourceFile;
  midiSource->readMidiEvents = _readMidiEventsFile;
  midiSource->freeMidiSourceData = _freeMidiEventsFile;

  MidiSourceFileData extraData = malloc(sizeof(MidiSourceFileDataMembers));
  extraData->fileHandle = NULL;
  midiSource->extraData = extraData;

  return midiSource;
}
