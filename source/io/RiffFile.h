//
// RiffFile.h - MrsWatson
// Created by Nik Reiman on 8/13/12.
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

#ifndef MrsWatson_RiffFile_h
#define MrsWatson_RiffFile_h

#include <stdio.h>

#include "base/CharString.h"
#include "base/Types.h"

typedef struct {
  char id[5];
  unsigned int size;
  byte *data;
} RiffChunkMembers;
typedef RiffChunkMembers *RiffChunk;

/**
 * Create a new RIFF chunk object
 * @return RiffChunk object
 */
RiffChunk newRiffChunk(void);

/**
 * Read the contents of the next chunk of a RIFF file into this object
 * @param self
 * @param fileHandle RIFF file, which should be opened for reading
 * @param readData If true, save the contents of the chunk in the RiffChunk's
 * data field. This is not always appropriate, for instance in the case of a
 * PCM file body. In this case, one usually wants to know the size of the data
 * chunk, but then to read bites from it in smaller blocks.
 * @return True if the chunk was successfully read
 */
boolByte riffChunkReadNext(RiffChunk self, FILE *fileHandle, boolByte readData);

/**
 * Test to see if this chunk's ID is equal to the given four character sequence
 * @param self
 * @param id String to compare to, should be exactly 4 characters
 * @return True if the ID's are equal, false otherwise
 */
boolByte riffChunkIsIdEqualTo(const RiffChunk self, const char *id);

/**
 * Free a RiffChunk object and its associated memory.
 * @param self
 */
void freeRiffChunk(RiffChunk self);

#endif
