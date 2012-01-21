//
// MidiSourceFile.c - MrsWatson
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
#include <string.h>
#include "MidiSource.h"
#include "MidiSourceFile.h"
#include "EventLogger.h"
#include "AudioSettings.h"
#include "PlatformUtilities.h"

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

static boolean _readMidiFileChunkHeader(FILE *midiFile, const char* expectedChunkId) {
  byte chunkId[5];
  memset(chunkId, 0, 5);
  size_t itemsRead = fread(chunkId, sizeof(byte), 4, midiFile);
  if(itemsRead < 4) {
    logError("Short read of MIDI file (at chunk ID)");
    return false;
  }
  else if(strncmp((char*)chunkId, expectedChunkId, 4)) {
    logError("MIDI file does not have valid chunk ID");
    return false;
  }
  else {
    return true;
  }
}

static boolean _readMidiFileHeader(FILE *midiFile, unsigned short *formatType, unsigned short *numTracks, unsigned short *timeDivision) {
  if(!_readMidiFileChunkHeader(midiFile, "MThd")) {
    return false;
  }

  unsigned int numBytesBuffer;
  size_t itemsRead = fread(&numBytesBuffer, sizeof(unsigned int), 1, midiFile);
  if(itemsRead < 1) {
    logError("Short read of MIDI file (at header, num items)");
    return false;
  }

  unsigned int numBytes = convertBigEndianIntToPlatform(numBytesBuffer);
  if(numBytes != 6) {
    logError("MIDI file has %d bytes in header chunk, expected 6", numBytes);
    return false;
  }

  unsigned short wordBuffer;
  itemsRead = fread(&wordBuffer, sizeof(unsigned short), 1, midiFile);
  if(itemsRead != 1) {
    logError("Short read of MIDI file (at header, format type)");
    return false;
  }
  *formatType = convertBigEndianShortToPlatform(wordBuffer);

  itemsRead = fread(&wordBuffer, sizeof(unsigned short), 1, midiFile);
  if(itemsRead != 1) {
    logError("Short read of MIDI file (at header, num tracks)");
    return false;
  }
  *numTracks = convertBigEndianShortToPlatform(wordBuffer);

  itemsRead = fread(&wordBuffer, sizeof(unsigned short), 1, midiFile);
  if(itemsRead != 1) {
    logError("Short read of MIDI file (at header, time division)");
    return false;
  }
  *timeDivision = convertBigEndianShortToPlatform(wordBuffer);

  return true;
}

static boolean _readMidiFileTrack(FILE *midiFile, const int trackNumber,
  const int timeDivision, const MidiFileTimeDivisionType divisionType,
  MidiSequence midiSequence) {
  if(!_readMidiFileChunkHeader(midiFile, "MTrk")) {
    return false;
  }

  unsigned int numBytesBuffer;
  size_t itemsRead = fread(&numBytesBuffer, sizeof(unsigned int), 1, midiFile);
  if(itemsRead < 1) {
    logError("Short read of MIDI file (at track %d header, num items)", trackNumber);
    return false;
  }

  // Read in the entire track in one pass and parse the events from the buffer data. Much easier
  // than having to call fread() for each event.
  size_t numBytes = (size_t)convertBigEndianIntToPlatform(numBytesBuffer);
  byte* trackData = malloc(numBytes);
  itemsRead = fread(trackData, 1, numBytes, midiFile);
  if(itemsRead != numBytes) {
    logError("Short read of MIDI file (at track %d)", trackNumber);
    return false;
  }

  unsigned long currentTimeInSampleFrames = 0;
  unsigned long unpackedVariableLength;
  byte* currentByte = trackData;
  byte* endByte = trackData + numBytes;
  while(currentByte < endByte) {
    // Unpack variable length timestamp
    unpackedVariableLength = *currentByte;
    if(unpackedVariableLength & 0x80) {
      unpackedVariableLength &= 0x7f;
      do {
        unpackedVariableLength = (unpackedVariableLength << 7) + (*(++currentByte) & 0x7f);
      } while(*currentByte & 0x80);
    }

    currentByte++;
    MidiEvent midiEvent = newMidiEvent();
    switch(*currentByte) {
      case 0xff:
        midiEvent->eventType = MIDI_TYPE_META;
        currentByte++;
        midiEvent->status = *(currentByte++);
        numBytes = *(currentByte++);
        midiEvent->extraData = malloc(numBytes);
        for(unsigned int i = 0; i < numBytes; i++) {
          midiEvent->extraData[i] = *(currentByte++);
        }
        break;
      case 0x7f:
        logUnsupportedFeature("Parsing MIDI sysex events from file");
        return false;
      default:
        midiEvent->eventType = MIDI_TYPE_REGULAR;
        midiEvent->status = *currentByte++;
        midiEvent->data1 = *currentByte++;
        // All regular MIDI events have 3 bytes except for program change and channel aftertouch
        if(!((midiEvent->status & 0xf0) == 0xc0 || (midiEvent->status & 0xf0) == 0xd0)) {
          midiEvent->data2 = *currentByte++;
        }
        break;
    }

    switch(divisionType) {
      case TIME_DIVISION_TYPE_TICKS_PER_BEAT:
      {
        // TODO: If the time signature is not 4/4, this calculation will be wrong
        double ticksPerSecond = (double)timeDivision * getTempo() / 60.0;
        double sampleFramesPerTick = getSampleRate() / ticksPerSecond;
        currentTimeInSampleFrames += (long)(unpackedVariableLength * sampleFramesPerTick);
      }
        break;
      case TIME_DIVISION_TYPE_FRAMES_PER_SECOND:
        // Actually, this should be caught when parsing the file type
        logUnsupportedFeature("Time division frames/sec");
        return false;
      case TIME_DIVISION_TYPE_INVALID:
      default:
        logInternalError("Invalid time division type");
        return false;
    }

    midiEvent->timestamp = currentTimeInSampleFrames;
    if(midiEvent->eventType == MIDI_TYPE_META) {
      logDebug("Parsed MIDI meta event of type 0x%02x", midiEvent->status);
      // TODO: Need to deal with certain types of meta events, like time signature, etc.
      // TODO: Also need to handle time signature changes, which really suck
      switch(midiEvent->status) {
        case 0x51:
          logUnsupportedFeature("Setting tempo from MIDI files");
          break;
        case 0x58:
          logUnsupportedFeature("Setting time signature from MIDI files");
          break;
        default:
          break;
      }
    }
    else {
      logDebug("MIDI event of type 0x%02x parsed at %ld", midiEvent->status, midiEvent->timestamp);
      appendMidiEventToSequence(midiSequence, midiEvent);
    }
  }

  free(trackData);
  return true;
}

static boolean _readMidiEventsFile(void* midiSourcePtr, MidiSequence midiSequence) {
  MidiSource midiSource = midiSourcePtr;
  MidiSourceFileData extraData = midiSource->extraData;
  unsigned short formatType, numTracks, timeDivision = 0;
  if(!_readMidiFileHeader(extraData->fileHandle, &formatType, &numTracks, &timeDivision)) {
    return false;
  }
  if(formatType != 0) {
    logUnsupportedFeature("MIDI file types other than 0");
    return false;
  }
  else if(formatType == 0 && numTracks != 1) {
    logError("MIDI file '%s' is of type 0, but contains %d tracks", midiSource->sourceName->data, numTracks);
    return false;
  }

  // Determine time division type
  if(timeDivision & 0x7fff) {
    extraData->divisionType = TIME_DIVISION_TYPE_TICKS_PER_BEAT;
  }
  else {
    extraData->divisionType = TIME_DIVISION_TYPE_FRAMES_PER_SECOND;
    logUnsupportedFeature("MIDI file with time division in frames/second");
    return false;
  }

  logDebug("MIDI file is type %d, has %d tracks, and time division %d (type %d)",
    formatType, numTracks, timeDivision, extraData->divisionType);

  for(int track = 0; track < numTracks; track++) {
    if(!_readMidiFileTrack(extraData->fileHandle, track, timeDivision, extraData->divisionType, midiSequence)) {
      return false;
    }
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
  extraData->divisionType = TIME_DIVISION_TYPE_INVALID;
  extraData->fileHandle = NULL;
  midiSource->extraData = extraData;

  return midiSource;
}
