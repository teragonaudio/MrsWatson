//
//  MidiSource.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MidiSource.h"
#include "EventLogger.h"
#include "StringUtilities.h"
#include "MidiSourceFile.h"

MidiSourceType guessMidiSourceType(const CharString midiSourceTypeString) {
  if(!isCharStringEmpty(midiSourceTypeString)) {
    const char* fileExtension = getFileExtension(midiSourceTypeString->data);
    if(fileExtension == NULL) {
      return MIDI_SOURCE_TYPE_INVALID;
    }
    else if(!strcasecmp(fileExtension, "mid") || !strcasecmp(fileExtension, "midi")) {
      return MIDI_SOURCE_TYPE_FILE;
    }
    else {
      logCritical("MIDI source '%s' does not match any supported type");
      return MIDI_SOURCE_TYPE_INVALID;
    }
  }
  else {
    logInternalError("MIDI source type was null");
    return MIDI_SOURCE_TYPE_INVALID;
  }
}

MidiSource newMidiSource(MidiSourceType midiSourceType, const CharString midiSourceName) {
  switch(midiSourceType) {
    case MIDI_SOURCE_TYPE_FILE:
      return newMidiSourceFile(midiSourceName);
    default:
      return NULL;
  }
}

void freeMidiSource(MidiSource midiSource) {
  midiSource->freeMidiSourceData(midiSource->extraData);
  freeCharString(midiSource->sourceName);
  free(midiSource);
}
