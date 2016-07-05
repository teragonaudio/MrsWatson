//
// RiffFile.c - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
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

#include "RiffFile.h"

#include "base/Endian.h"

#include <stdlib.h>
#include <string.h>

RiffChunk newRiffChunk(void) {
  RiffChunk chunk = (RiffChunk)malloc(sizeof(RiffChunkMembers));
  memset(chunk->id, 0, 5);
  chunk->size = 0;
  chunk->data = NULL;
  return chunk;
}

boolByte riffChunkReadNext(RiffChunk self, FILE *fileHandle,
                           boolByte readData) {
  size_t itemsRead = 0;
  byte *chunkSize;

  if (fileHandle != NULL) {
    itemsRead = fread(self->id, 1, 4, fileHandle);

    if (itemsRead != 4) {
      return false;
    }

    chunkSize = (byte *)malloc(sizeof(byte) * 4);
    memset(chunkSize, 0, 4);
    itemsRead = fread(chunkSize, 1, 4, fileHandle);

    if (itemsRead != 4) {
      free(chunkSize);
      return false;
    }

    self->size = convertByteArrayToUnsignedInt(chunkSize);
    free(chunkSize);

    if (self->size > 0 && readData) {
      self->data = (byte *)malloc(self->size);
      itemsRead = fread(self->data, 1, self->size, fileHandle);

      if (itemsRead != self->size) {
        return false;
      }
    }
  }

  return (boolByte)!feof(fileHandle);
}

boolByte riffChunkIsIdEqualTo(const RiffChunk self, const char *id) {
  return (boolByte)(strncmp(self->id, id, 4) == 0);
}

void freeRiffChunk(RiffChunk self) {
  if (self->data) {
    free(self->data);
  }

  free(self);
}
