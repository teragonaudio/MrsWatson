//
// RiffFile.c - MrsWatson
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

#include <stdlib.h>
#include <string.h>
#include "RiffFile.h"
#include "PlatformUtilities.h"

RiffChunk newRiffChunk(void) {
  RiffChunk chunk = (RiffChunk)malloc(sizeof(RiffChunkMembers));
  memset(chunk->id, 0, 5);
  chunk->size = 0;
  chunk->data = NULL;
  return chunk;
}

boolByte readNextChunk(FILE* fileHandle, RiffChunk outChunk, boolByte readData) {
  unsigned int itemsRead = 0;
  byte* chunkSize = (byte*)malloc(sizeof(byte) * 4);

  if(fileHandle != NULL && outChunk != NULL) {
    itemsRead = fread(outChunk->id, 1, 4, fileHandle);
    if(itemsRead != 4) {
      return false;
    }

    memset(chunkSize, 0, 4);
    itemsRead = fread(chunkSize, 1, 4, fileHandle);
    if(itemsRead != 4) {
      return false;
    }
    outChunk->size = convertByteArrayToUnsignedInt(chunkSize);

    if(outChunk->size > 0 && readData) {
      outChunk->data = (byte*)malloc(outChunk->size);
      itemsRead = fread(outChunk->data, 1, outChunk->size, fileHandle);
      if(itemsRead != outChunk->size) {
        return false;
      }
    }
  }

  return !feof(fileHandle);
}

boolByte isChunkIdEqualTo(const RiffChunk chunk, const char*id) {
  return strncmp(chunk->id, id, 4) == 0;
}

void freeRiffChunk(RiffChunk chunk) {
  if(chunk->data) {
    free(chunk->data);
  }
  free(chunk);
}
