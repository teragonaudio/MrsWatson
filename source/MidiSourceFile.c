//
//  MidiSourceFile.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

static boolean _readMidiFileHeader(FILE *midiFile,
  unsigned short *formatType, unsigned short *numTracks, unsigned short *timeDivision) {
  byte chunkId[5];
  memset(chunkId, 0, 5);
  size_t itemsRead = fread(chunkId, sizeof(byte), 4, midiFile);
  if(itemsRead < 4) {
    logError("Short read of MIDI file (at header, chunk ID)");
    return false;
  }
  else if(strncmp((char*)chunkId, "MThd", 4)) {
    logError("MIDI file does not have valid header chunk ID");
    return false;
  }

  unsigned int numBytesBuffer;
  itemsRead = fread(&numBytesBuffer, sizeof(unsigned int), 1, midiFile);
  if(itemsRead < 1) {
    logError("Short read of MIDI file (at header, num items)");
    return false;
  }

  unsigned int numBytes = htonl(numBytesBuffer);
  if(numBytes != 6) {
    logError("MIDI file has %d bytes in header chunk, expected 6", numBytes);
    return false;
  }

  unsigned short wordBuffer;
  itemsRead = fread(&wordBuffer, sizeof(unsigned short), 2, midiFile);
  if(itemsRead != 2) {
    logError("Short read of MIDI file (at header, format type");
    return false;
  }
  *formatType = htons(wordBuffer);

  itemsRead = fread(&wordBuffer, sizeof(unsigned short), 2, midiFile);
  if(itemsRead != 2) {
    logError("Short read of MIDI file (at header, num tracks)");
    return false;
  }
  *numTracks = htons(wordBuffer);

  itemsRead = fread(&wordBuffer, sizeof(unsigned short), 2, midiFile);
  if(itemsRead != 2) {
    logError("Short read of MIDI file (at header, time division)");
    return false;
  }
  *timeDivision = htons(wordBuffer);

  return true;
}

static boolean _readMidiEventsFile(void* midiSourcePtr, MidiSequence midiSequence) {
  MidiSource midiSource = midiSourcePtr;
  MidiSourceFileData extraData = midiSource->extraData;
  unsigned short formatType, numTracks, timeDivision = 0;
  if(!_readMidiFileHeader(extraData->fileHandle, &formatType, &numTracks, &timeDivision)) {
    return false;
  }
  logDebug("MIDI file is type %d, has %d tracks, and time division %d", formatType, numTracks, timeDivision);
  if(formatType != 0) {
    logUnsupportedFeature("MIDI file types other than 0");
    return false;
  }

  return false;
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
