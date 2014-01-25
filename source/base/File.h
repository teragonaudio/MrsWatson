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

#include <stdio.h>
#include <stdlib.h>
#include "base/CharString.h"
#include "base/LinkedList.h"
#include "base/Types.h"

#if UNIX
#define PATH_DELIMITER '/'
#define ROOT_DIRECTORY "/"
#elif WINDOWS
#define PATH_DELIMITER '\\'
#define ROOT_DIRECTORY "C:\\"
#endif

typedef enum {
  kFileTypeFile,
  kFileTypeDirectory,
  kFileTypeInvalid
} FileType;

typedef enum {
  kFileOpenModeClosed,
  kFileOpenModeRead,
  kFileOpenModeWrite,
  kFileOpenModeInvalid
} FileOpenMode;

typedef struct {
  CharString absolutePath;
  FileType fileType;

  /** Private */
  FILE *_fileHandle;
  /** Private */
  FileOpenMode _openMode;
} FileMembers;
typedef FileMembers* File;

/**
 * @return New empty file object
 */
File newFile(void);

/**
 * Create a new file object which points to a given path. If something exists at
 * the path, then this object will be initialized with the correct type.
 * @param path Object path. If this is not an absolute path, it is assumed to
 * be relative to the current directory.
 * @return New file object
 */
File newFileWithPath(const CharString path);

/**
 * Create a new file object which points to a given path. If something exists at
 * the path, then this object will be initialized with the correct type.
 * @param path Object path. If this is not an absolute path, it is assumed to
 * be relative to the current directory.
 * @return New file object
 */
File newFileWithPathCString(const char* path);

/**
 * Create a new file object which points to a path under another directory. If
 * something exists at the path, then this object will be initialized with the
 * correct type.
 * @param parent Directory which the object is located under. If parent does not
 * exist or is not a directory, this will return NULL.
 * @param path Object path, relative to the parent. This may not be an absolute
 * path.
 * @return New file object, or NULL if this object could not be created.
 */
File newFileWithParent(const File parent, const CharString path);

/**
 * Check to see if the path referenced by this object exists on disk.
 * @param self
 * @return True if the object exists on disk
 */
boolByte fileExists(File self);

/**
 * Create a file object on disk of the given type. This call will fail if an
 * object already exists on the disk at the path pointed to by this file.
 * @param self
 * @param fileType Type of object to create
 * @return True if the object was created
 */
boolByte fileCreate(File self, const FileType fileType);

/**
 * Copy a file object to a new location. If this file is a directory, it will
 * be copied recursively. This call will fail if the destination does not exist.
 * @param self
 * @param destination Target destination to copy objects to.
 * @return File object for copied object, or NULL if it could not be copied. The
 * caller must free this object when finished with it.
 */
File fileCopyTo(File self, const File destination);

/**
 * Remove the file from disk. If this object is a directory, then it will be
 * removed recursively.
 * Note: On Windows, you must make sure that all files inside of this directory
 * are closed, otherwise the operation will fail.
 * @param self
 * @return True if the object could be removed
 */
boolByte fileRemove(File self);

/**
 * List the contents of a directory, non-recursively. Special entries such as
 * "." and ".." are not included in the listing, nor are hidden dotfiles.
 * @param self
 * @return A linked list of File objects, or NULL on error. The caller must
 * release this memory using freeLinkedListAndItems.
 */
LinkedList fileListDirectory(File self);

/**
 * Return the size of a file in bytes.
 * @param self
 * @return Number of bytes in the file, or 0 if this object does not exist or
 * is not a file.
 */
size_t fileGetSize(File self);

/**
 * Read the contents of an entire file into a string. If the file had previously
 * been opened for writing, then it will be flushed, closed, and reopened for
 * reading.
 * @param self
 * @return CharString containing file contents, or NULL if an error occurred.
 */
CharString fileReadContents(File self);

/**
 * Read the contents of an entire file line-by-line. The lines are returned in
 * a linked list, which the caller must free when finished (and should use the
 * freeLinkedListAndItems() method with freeCharString as the second argument).
 * @param self
 * @return LinkedList containing a CharString for each line, or NULL if an
 * error occurred. Note that the newline character is removed from the lines.
 */
LinkedList fileReadLines(File self);

/**
 * Read part of a binary file into a raw byte array. If end of file is reached,
 * then an array of numBytes is still delivered with the extra bytes initialized
 * to 0. This is most useful when reading raw PCM data from disk. If the file
 * had previously been opened for writing, then it will be flushed, closed, and
 * reopened for reading.
 * @param self
 * @param numBytes Number of bytes to deliver, at most. If this is greater than
 * the size of the actual file, then the entire file will be read. If this is
 * zero, then NULL will be returned.
 * @return An initialized array of numBytes bytes with the data, or NULL if an
 * error occurred.
 */
void* fileReadBytes(File self, size_t numBytes);

/**
 * Write a string to file. The first time this function is called, the file will
 * be opened for write mode, truncating any data present there if the file
 * already exists. File buffers are not flushed until fileClose() is called.
 * @param self
 * @param data String to write
 * @return True if the data was written
 */
boolByte fileWrite(File self, const CharString data);

/**
 * Write a binary byte array to disk. The first time this function is called,
 * the file will be opened for write mode, truncating any data present there if
 * the file already exists. File buffers are not flushed until fileClose() is
 * called.
 * @param self
 * @param data Binary data to write
 * @param numBytes Number of bytes to write
 * @return True if the data could be written
 */
boolByte fileWriteBytes(File self, const void* data, size_t numBytes);

/**
 * Get the file basename, for example "/foo/bar" would return "bar" (regardless
 * of whether "bar" is a file or directory).
 * @param self
 * @return CharString containing basename. The caller must free this memory when
 * finished with it.
 */
CharString fileGetBasename(File self);

/**
 * Get a file's parent directory.
 * @param self
 * @return File representing the parent directory. The caller must free this
 * object when finished with it.
 */
File fileGetParent(File self);

/**
 * Return a pointer to the file's extension.
 * @param self
 * @return Pointer to file extension, or NULL if the file has no extension or
 * an error occurred.
 */
CharString fileGetExtension(File self);

/**
 * Close a file and flush its buffers to disk.
 * @param self
 */
void fileClose(File self);

/**
 * Free a file object and any associated resources
 * @param self
 */
void freeFile(File self);

#endif
