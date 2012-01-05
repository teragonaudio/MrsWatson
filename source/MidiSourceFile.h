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

typedef struct {
  FILE* fileHandle;
} MidiSourceFileDataMembers;

typedef MidiSourceFileDataMembers* MidiSourceFileData;

MidiSource newMidiSourceFile(const CharString midiSourceName);

#endif
