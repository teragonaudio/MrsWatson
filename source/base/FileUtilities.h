//
// FileUtilities.h - MrsWatson
// Created by Nik Reiman on 9/20/12.
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

#ifndef MrsWatson_FileUtilities_h
#define MrsWatson_FileUtilities_h

#include "base/CharString.h"
#include "base/LinkedList.h"
#include "base/PlatformUtilities.h"

#if UNIX
#define PATH_DELIMITER '/'
#define ROOT_DIRECTORY "/"
#elif WINDOWS
#define PATH_DELIMITER '\\'
#define ROOT_DIRECTORY "C:\\"
#endif

/** DEPRECATED */
boolByte _fileExists(const char* path);
/** DEPRECATED */
boolByte copyFileToDirectory(const CharString fileAbsolutePath, const CharString directoryAbsolutePath);

/** DEPRECATED */
boolByte makeDirectory(const CharString absolutePath);
/** DEPRECATED */
LinkedList listDirectory(const CharString directory);
/** DEPRECATED */
boolByte removeDirectory(const CharString absolutePath);

// const char* is used here as it is assumed that the extensions will be defined by the preprocessor
// for given platforms, not kept in stack memory.
/** DEPRECATED */
void buildAbsolutePath(const CharString directory, const CharString file, const char* fileExtension, CharString outString);
/** DEPRECATED */
void convertRelativePathToAbsolute(const CharString file, CharString outString);
/** DEPRECATED */
boolByte isAbsolutePath(const CharString path);

/** DEPRECATED */
const char* getFileBasename(const char* filename);
/** DEPRECATED */
const char* getFileExtension(const char* filename);
/** DEPRECATED */
void getFileDirname(const CharString filename, CharString outString);

#endif
