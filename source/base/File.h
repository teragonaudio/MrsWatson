// 
// File.h - MrsWatson 
// Created by Nik Reiman on 09 Dec 12. 
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

#ifndef MrsWatson_File_h
#define MrsWatson_File_h

#if USE_NEW_FILE_API

#include <stdlib.h>
#include "base/CharString.h"
#include "base/LinkedList.h"
#include "base/Types.h"

typedef enum {
  kFileTypeFile,
  kFileTypeDirectory,
  kFileTypeInvalid
} FileType;

typedef struct {
  CharString absolutePath;
  FileType fileType;
} FileMembers;
typedef FileMembers* File;

File newFile(void);
File newFileWithPath(CharString path);

boolByte fileCreate(File self);
boolByte fileCopyTo(File self, const File destination);
int fileListContents(File self, LinkedList outItems);
boolByte fileRemove(File self);

CharString fileGetBasename(File self);
CharString fileGetDirname(File self);
CharString fileGetExtension(File self);

boolByte fileExists(File self);

void freeFile(File file);

#endif
#endif
