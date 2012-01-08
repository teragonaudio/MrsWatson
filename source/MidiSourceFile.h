//
//  MidiSourceFile.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include "MidiSource.h"

#ifndef MrsWatson_MidiSourceFile_h
#define MrsWatson_MidiSourceFile_h

typedef enum {
  TIME_DIVISION_TYPE_INVALID,
  TIME_DIVISION_TYPE_TICKS_PER_BEAT,
  TIME_DIVISION_TYPE_FRAMES_PER_SECOND,
  NUM_TIME_DIVISION_TYPES
} MidiFileTimeDivisionType;

typedef struct {
  FILE* fileHandle;
  MidiFileTimeDivisionType divisionType;
} MidiSourceFileDataMembers;

typedef MidiSourceFileDataMembers* MidiSourceFileData;

MidiSource newMidiSourceFile(const CharString midiSourceName);

#endif
