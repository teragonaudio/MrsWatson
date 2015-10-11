//
// File.c - MrsWatson
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

// Must be declared before stdlib, shouldn't have any effect on Windows builds
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "base/File.h"
#include "logging/EventLogger.h"

#if WINDOWS
#include <Windows.h>
#include <Shellapi.h>
#elif UNIX
#if LINUX
#include <errno.h>
#elif MACOSX
#include <mach-o/dyld.h>
#endif

#include <dirent.h>
#include <ftw.h>
#include <unistd.h>
#endif

static const int kFileNameStringLength = 1024;
static const int kFileMaxRecursionDepth = 32;
static const char *kFileNameInvalidCharacters = "*:?<>|";

static boolByte _isAbsolutePath(const CharString path)
{
#if WINDOWS

    if (path->capacity > 3) {
        // Check for strings which start with a drive letter, ie C:\file
        if (path->data[1] == ':' && path->data[2] == PATH_DELIMITER) {
            return true;
        }
        // Check for network paths (like "\\SERVER\file") or absolute paths (like "\Users")
        if (path->data[0] == PATH_DELIMITER) {
            return true;
        }
    }

#else

    if (path->capacity > 1 && path->data[0] == PATH_DELIMITER) {
        return true;
    }

#endif
    return false;
}

static CharString _convertRelativePathToAbsolute(const CharString path)
{
    CharString result = newCharStringWithCapacity(kFileNameStringLength);
    CharString currentDirectory = fileGetCurrentDirectory();
    snprintf(result->data, result->capacity, "%s%c%s", currentDirectory->data, PATH_DELIMITER, path->data);
    freeCharString(currentDirectory);
    return result;
}

static CharString _buildAbsolutePath(const CharString parentDir, const CharString filename)
{
    CharString result = NULL;
    CharString parentDirAbsPath = NULL;

    if (parentDir == NULL || charStringIsEmpty(parentDir)) {
        logWarn("Attempt to build absolute path with empty directory");
        return result;
    } else if (filename == NULL || charStringIsEmpty(filename)) {
        logWarn("Attempt to build absolute path with empty file");
        return result;
    } else if (_isAbsolutePath(filename)) {
        result = newCharStringWithCapacity(kFileNameStringLength);
        charStringCopy(result, filename);
        return result;
    }

    if (_isAbsolutePath(parentDir)) {
        parentDirAbsPath = newCharStringWithCapacity(kFileNameStringLength);
        charStringCopy(parentDirAbsPath, parentDir);
    } else {
        parentDirAbsPath = _convertRelativePathToAbsolute(parentDir);
    }

    result = newCharStringWithCapacity(kFileNameStringLength);
    snprintf(result->data, result->capacity, "%s%c%s", parentDirAbsPath->data, PATH_DELIMITER, filename->data);
    freeCharString(parentDirAbsPath);
    return result;
}

static boolByte _isDirectory(const CharString path)
{
    boolByte result = false;

#if UNIX
    struct stat buffer;

    if (stat(path->data, &buffer) == 0) {
        result = (boolByte)S_ISDIR(buffer.st_mode);
    }

#elif WINDOWS
    DWORD fileAttributes = GetFileAttributesA((LPCSTR)path->data);

    if (fileAttributes != INVALID_FILE_ATTRIBUTES) {
        result = (boolByte)(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    }

#endif

    return result;
}

static boolByte _pathContainsInvalidChars(const CharString path)
{
    size_t i, j = 0;
#if WINDOWS

    // The colon is not allowed in pathnames (even on Windows), however it is
    // present in absolute pathnames for the drive letter. Thus we must skip
    // the first 3 characters of the path when dealing with absolute paths on
    // this platform.
    if (_isAbsolutePath(path)) {
        j = 2;
    }

#endif

    if (path != NULL && path->data != NULL && strlen(path->data) > j) {
        for (i = 0; i < strlen(kFileNameInvalidCharacters); ++i) {
            if (strchr(path->data + j, kFileNameInvalidCharacters[i]) != NULL) {
                return true;
            }
        }
    }

    return false;
}

File newFile(void)
{
    File result = (File)malloc(sizeof(FileMembers));
    result->absolutePath = newCharStringWithCapacity(kFileNameStringLength);
    result->fileType = kFileTypeInvalid;
    result->_fileHandle = NULL;
    result->_openMode = kFileOpenModeClosed;
    return result;
}

File newFileWithPath(const CharString path)
{
    File result = newFile();
    CharString currentDirectory = NULL;
    CharString absolutePath = NULL;

    if (path != NULL && !charStringIsEmpty(path)) {
        if (_isAbsolutePath(path)) {
            charStringCopy(result->absolutePath, path);
        } else if (_pathContainsInvalidChars(path)) {
            logWarn("Could not create file/directory with name '%s'", path->data);
            freeFile(result);
            return NULL;
        } else {
            currentDirectory = fileGetCurrentDirectory();

            if (currentDirectory == NULL) {
                logWarn("Could not create file relative to current directory");
                freeFile(result);
                return NULL;
            }

            absolutePath = _buildAbsolutePath(currentDirectory, path);
            charStringCopy(result->absolutePath, absolutePath);
            freeCharString(currentDirectory);
            freeCharString(absolutePath);
        }

        if (fileExists(result)) {
            result->fileType = _isDirectory(result->absolutePath) ? kFileTypeDirectory : kFileTypeFile;
        }
    }

    return result;
}

File newFileWithPathCString(const char *path)
{
    File result = NULL;

    if (path != NULL) {
        CharString pathString = newCharStringWithCString(path);
        result = newFileWithPath(pathString);
        freeCharString(pathString);
    }

    return result;
}

File newFileWithParent(const File parent, const CharString path)
{
    File result = NULL;
    CharString absolutePath = NULL;

    if (parent == NULL) {
        logWarn("Cannot create file/directory with null parent");
        return NULL;
    } else if (!fileExists(parent)) {
        logWarn("Cannot create file/directory under non-existent parent");
        return NULL;
    } else if (!_isDirectory(parent->absolutePath)) {
        logWarn("Cannot create file/directory under non-directory parent '%s'", parent->absolutePath->data);
        return NULL;
    } else if (path == NULL || charStringIsEmpty(path)) {
        logWarn("Cannot create empty file/directory name with parent");
        return NULL;
    } else if (_pathContainsInvalidChars(path)) {
        logWarn("Could not create file/directory with name '%s'", path->data);
        freeFile(result);
        return NULL;
    } else if (_isAbsolutePath(path)) {
        logWarn("Cannot create file '%s' with absolute directory under a parent", path->data);
        freeFile(result);
        return NULL;
    }

    result = newFile();
    absolutePath = _buildAbsolutePath(parent->absolutePath, path);
    charStringCopy(result->absolutePath, absolutePath);
    freeCharString(absolutePath);

    if (fileExists(result)) {
        result->fileType = _isDirectory(result->absolutePath) ? kFileTypeDirectory : kFileTypeFile;
    }

    return result;
}

boolByte fileExists(File self)
{
#if WINDOWS
    // Visual Studio's compiler is not C99 compliant, so variable declarations
    // need to be at the top.
    unsigned long fileAttributes;
#endif

    // Normally, we don't do paranoid sanity checks for self != NULL, but in this
    // case it's useful since the old file API had a different calling convention
    // which supported calling fileExists() with NULL.
    if (self == NULL || self->absolutePath == NULL) {
        return false;
    }

#if UNIX
    struct stat fileStat;
    boolByte result = (boolByte)(stat(self->absolutePath->data, &fileStat) == 0);
    return result;

#elif WINDOWS
    fileAttributes = GetFileAttributesA((LPCSTR)self->absolutePath->data);

    if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    return true;
#else
    return false;
#endif
}

boolByte fileCreate(File self, const FileType fileType)
{
    if (fileExists(self)) {
        return false;
    } else if (charStringIsEmpty(self->absolutePath)) {
        logWarn("Attempt to create file/directory without a path");
        return false;
    }

    switch (fileType) {
    case kFileTypeFile:
        self->_fileHandle = fopen(self->absolutePath->data, "wb");

        if (self->_fileHandle != NULL) {
            self->fileType = kFileTypeFile;
            self->_openMode = kFileOpenModeWrite;
            return true;
        }

        break;

    case kFileTypeDirectory:
#if UNIX
        if (mkdir(self->absolutePath->data, 0755) == 0) {
#elif WINDOWS

        if (CreateDirectoryA(self->absolutePath->data, NULL)) {
#endif
            self->fileType = kFileTypeDirectory;
            return true;
        }

        break;

    default:
        break;
    }

    return false;
}

static File _copyFileToDirectory(File self, const File destination) {
    File result = NULL;
    CharString basename = NULL;
    void *selfContents = NULL;
    size_t selfSize = 0;

    // Close and re-open file to make sure that we start reading at the beginning
    fileClose(self);
    self->_fileHandle = fopen(self->absolutePath->data, "rb");

    if (self->_fileHandle == NULL) {
        logError("Could not re-open file for reading during copy");
        return NULL;
    }

    basename = fileGetBasename(self);

    if (basename == NULL) {
        logError("Could not get basename during copy");
    } else {
        result = newFileWithParent(destination, basename);
        freeCharString(basename);

        if (result == NULL) {
            logError("Could not create destination file object");
            return NULL;
        }

        if (!fileCreate(result, kFileTypeFile)) {
            logError("Could not create destination file");
            freeFile(result);
            return NULL;
        }

        selfSize = fileGetSize(self);

        if (selfSize == 0) {
            // If the source file is empty, then creating the result file is good
            // enough and we can return here.
            logDebug("Source file during copy is 0 bytes");
            return result;
        }

        selfContents = fileReadBytes(self, selfSize);

        if (selfContents == NULL) {
            logError("Could not read source file during copy");
            freeFile(result);
            return NULL;
        }

        if (!fileWriteBytes(result, selfContents, selfSize)) {
            logError("Could not copy source file");
            freeFile(result);
            free(selfContents);
            return NULL;
        }
        free(selfContents);
    }

    return result;
}

static File _copyDirectoryToDirectory(File self, const File destination) {
    File result = NULL;
    // Get the basename first, because if it fails then there's no point in doing
    // the actual copy.
    CharString basename = fileGetBasename(self);
#if WINDOWS
    SHFILEOPSTRUCT fileOperation;
#endif

    if (basename == NULL) {
        logError("Could not get basename from directory during copy");
        return NULL;
    }

#if UNIX
    // Using nftw() is a real pain here, because the recursive callback will start
    // at the top of the tree and work back up, meaning that for each file we'd
    // need to recursively mkdir the basename, etc.
    // So this is a bit lazy but actually more foolproof
    CharString copyCommand = newCharString();
    snprintf(copyCommand->data, copyCommand->capacity, "/bin/cp -r \"%s\" \"%s\"",
             self->absolutePath->data, destination->absolutePath->data);
    int systemResult = system(copyCommand->data);
    freeCharString(copyCommand);

    if (WEXITSTATUS(systemResult) == 0) {
        result = newFileWithParent(destination, basename);

        if (result == NULL) {
            logError("Copied '%s' to '%s', but could not create a File object for the result",
                     self->absolutePath->data, destination->absolutePath->data);
            freeCharString(basename);
            return NULL;
        }
    } else {
        logError("Could not copy directory '%s' to '%s'",
                 self->absolutePath->data, destination->absolutePath->data);
    }

#elif WINDOWS
    memset(&fileOperation, 0, sizeof(fileOperation));
    fileOperation.wFunc = FO_COPY;
    fileOperation.pFrom = self->absolutePath->data;
    fileOperation.pTo = destination->absolutePath->data;
    fileOperation.fFlags = FOF_NO_UI;

    if (SHFileOperationA(&fileOperation) == 0) {
        result = newFileWithParent(destination, basename);
    }

#endif

    freeCharString(basename);
    return result;
}

File fileCopyTo(File self, const File destination) {
    File result = NULL;

    if (destination == NULL || !fileExists(destination)) {
        logError("Attempt to copy file/directory to invalid destination");
        return NULL;
    } else if (!_isDirectory(destination->absolutePath)) {
        logError("Attempt to copy file/directory to non-directory destination");
        return NULL;
    } else if (!fileExists(self)) {
        logError("Attempt to copy non-existent file");
        return NULL;
    }

    switch (self->fileType) {
    case kFileTypeFile:
        result = _copyFileToDirectory(self, destination);
        break;

    case kFileTypeDirectory:
        result = _copyDirectoryToDirectory(self, destination);
        break;

    default:
        logError("Attempt to copy invalid file type");
        break;
    }

    return result;
}

#if UNIX
static int _removeCallback(const char *path, const struct stat * fileState, int typeflag, struct FTW * ftwBuffer) {
    int result = remove(path);

    if (result != 0) {
        logWarn("Could not remove '%s'", path);
    }

    return result;
}
#endif

boolByte fileRemove(File self) {
    boolByte result = false;
#if WINDOWS
    SHFILEOPSTRUCTA fileOperation = {0};
#endif

    if (fileExists(self)) {
        switch (self->fileType) {
        case kFileTypeFile:
            // Yes, this seems a bit silly, but otherwise we threaten to leak resources
            fileClose(self);
            result = (boolByte)(remove(self->absolutePath->data) == 0);
            break;

        case kFileTypeDirectory:
#if UNIX
            result = (boolByte)(nftw(self->absolutePath->data, _removeCallback, kFileMaxRecursionDepth, FTW_DEPTH | FTW_PHYS) == 0);
#elif WINDOWS
            memset(&fileOperation, 0, sizeof(fileOperation));
            fileOperation.wFunc = FO_DELETE;
            fileOperation.pFrom = self->absolutePath->data;
            fileOperation.pTo = NULL;
            fileOperation.fFlags = FOF_NO_UI | FOF_NOCONFIRMATION | FOF_SILENT;
            result = (SHFileOperationA(&fileOperation) == 0);
#endif
            break;

        default:
            break;
        }
    }

    if (result) {
        self->fileType = kFileTypeInvalid;
    }

    return result;
}

LinkedList fileListDirectory(File self) {
    LinkedList items = newLinkedList();
    CharString path;
    File file;

#if UNIX
    DIR *directoryPtr = opendir(self->absolutePath->data);

    if (directoryPtr == NULL) {
        freeLinkedList(items);
        return 0;
    }

    struct dirent *entry;

    while ((entry = readdir(directoryPtr)) != NULL) {
        if (entry->d_name[0] != '.') {
            path = newCharStringWithCString(entry->d_name);
            file = newFileWithParent(self, path);
            linkedListAppend(items, file);
            freeCharString(path);
        }
    }

    closedir(directoryPtr);

#elif WINDOWS
    WIN32_FIND_DATAA findData;
    HANDLE findHandle;
    CharString searchString = newCharStringWithCapacity(kFileNameStringLength);

    snprintf(searchString->data, searchString->capacity, "%s\\*", self->absolutePath->data);
    findHandle = FindFirstFileA((LPCSTR)(searchString->data), &findData);
    freeCharString(searchString);

    if (findHandle == INVALID_HANDLE_VALUE) {
        freeLinkedList(items);
        return 0;
    }

    do {
        if (findData.cFileName[0] != '.') {
            path = newCharStringWithCString(findData.cFileName);
            file = newFileWithParent(self, path);
            linkedListAppend(items, file);
            freeCharString(path);
        }
    } while (FindNextFileA(findHandle, &findData) != 0);

    FindClose(findHandle);

#else
    logUnsupportedFeature("List directory contents");
#endif

    return items;
}

size_t fileGetSize(File self) {
    size_t result = 0;

#if UNIX
    struct stat fileStat;

    if (self->absolutePath == NULL) {
        return 0;
    }

    if (stat(self->absolutePath->data, &fileStat) == 0) {
        if (S_ISREG(fileStat.st_mode)) {
            // Yes, this will result in a loss of precision, but both freah and fwrite
            // take a size_t argument, which is what this function is mostly used for.
            result = (size_t)fileStat.st_size;
        }
    }

#elif WINDOWS
    HANDLE handle = CreateFileA(self->absolutePath->data, GENERIC_READ, 0, NULL,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (handle != INVALID_HANDLE_VALUE) {
        result = GetFileSize(handle, NULL);
        CloseHandle(handle);
    }

#else
    logUnsupportedFeature("Get file size");
#endif

    return result;
}

CharString fileReadContents(File self) {
    CharString result = NULL;
    size_t fileSize = 0;
    size_t itemsRead = 0;

    if (self->fileType != kFileTypeFile) {
        logError("Attempt to read contents from non-file object '%s'", self->absolutePath->data);
        return NULL;
    }

    // Windows has problems opening files multiple times (which is done internally in
    // fileGetSize() via CreateFile), so we must close the file first, calculate size,
    // and then reopen it for reading.
    fileClose(self);
    fileSize = (size_t)fileGetSize(self);
    fileClose(self);

    if (self->_fileHandle == NULL) {
        self->_fileHandle = fopen(self->absolutePath->data, "rb");

        if (self->_fileHandle == NULL) {
            logError("Could not open '%s' for reading", self->absolutePath->data);
            return NULL;
        } else {
            self->_openMode = kFileOpenModeRead;
        }
    }

    if (fileSize > 0) {
        result = newCharStringWithCapacity(fileSize + 1);
        itemsRead = fread(result->data, 1, fileSize, self->_fileHandle);

        if (itemsRead != fileSize) {
            logError("Expected to read %d items, read %d items instead", fileSize, itemsRead);
        }
    }

    return result;
}

LinkedList fileReadLines(File self) {
    LinkedList result = NULL;
    CharString line = NULL;
    boolByte done = false;
    char *newline = NULL;

    if (self->fileType != kFileTypeFile) {
        logError("Attempt to read contents from non-file object '%s'", self->absolutePath->data);
        return NULL;
    }

    if (self->_openMode != kFileOpenModeRead && self->_fileHandle != NULL) {
        fileClose(self);
    }

    if (self->_fileHandle == NULL) {
        self->_fileHandle = fopen(self->absolutePath->data, "rb");

        if (self->_fileHandle == NULL) {
            logError("Could not open '%s' for reading", self->absolutePath->data);
            return NULL;
        } else {
            self->_openMode = kFileOpenModeRead;
        }
    }

    result = newLinkedList();

    while (!done) {
        line = newCharString();

        if (fgets(line->data, (int)line->capacity, self->_fileHandle) == NULL) {
            freeCharString(line);
            done = true;
        } else {
            newline = strrchr(line->data, '\n');

            if (newline != NULL) {
                *newline = '\0';
            }

            // Also trim these characters, in case the file has Windows-style newlines
            newline = strrchr(line->data, '\r');

            if (newline != NULL) {
                *newline = '\0';
            }

            linkedListAppend(result, line);
        }
    }

    return result;
}

void *fileReadBytes(File self, size_t numBytes) {
    void *result = NULL;
    size_t itemsRead = 0;

    if (numBytes == 0) {
        // Here we log an error (instead of an info message, as is done with the
        // same type of error during writes) because the caller is probably
        // expecting a non-NULL object back.
        logError("Attempt to read 0 bytes from file");
        return NULL;
    }

    if (self->fileType != kFileTypeFile) {
        logError("Attempt to read bytes from non-file object '%s'", self->absolutePath->data);
        return NULL;
    }

    if (self->_openMode != kFileOpenModeRead && self->_fileHandle != NULL) {
        fileClose(self);
    }

    if (self->_fileHandle == NULL) {
        self->_fileHandle = fopen(self->absolutePath->data, "rb");

        if (self->_fileHandle == NULL) {
            logError("Could not open '%s' for reading", self->absolutePath->data);
            return NULL;
        } else {
            self->_openMode = kFileOpenModeRead;
        }
    }

    if (numBytes > 0) {
        result = malloc(numBytes + 1);
        memset(result, 0, numBytes + 1);
        itemsRead = fread(result, 1, numBytes, self->_fileHandle);

        if (itemsRead != numBytes) {
            logError("Expected to read %d items, read %d items instead", numBytes, itemsRead);
        }
    }

    return result;
}

boolByte fileWrite(File self, const CharString data) {
    return fileWriteBytes(self, data->data, strlen(data->data));
}

boolByte fileWriteBytes(File self, const void *data, size_t numBytes) {
    size_t itemsWritten = 0;

    if (numBytes == 0) {
        // Here we only log an info message, as this isn't such a critical error,
        // but it is rather weird.
        logInfo("Attempt to write 0 bytes to file");
        return false;
    }

    if (!fileExists(self)) {
        logError("Attempt to write to non-existent file");
        return false;
    }

    if (self->_openMode != kFileOpenModeWrite && self->_fileHandle != NULL) {
        fileClose(self);
    }

    if (self->_fileHandle == NULL) {
        self->_fileHandle = fopen(self->absolutePath->data, "wb");

        if (self->_fileHandle == NULL) {
            logError("Could not open '%s' for writing", self->absolutePath->data);
            return false;
        } else {
            self->_openMode = kFileOpenModeWrite;
        }
    }

    itemsWritten = fwrite(data, 1, numBytes, self->_fileHandle);
    return (boolByte)(itemsWritten == numBytes);
}

CharString fileGetBasename(File self) {
    CharString result = NULL;
    char *lastDelimiter = NULL;

    if (self->absolutePath == NULL || charStringIsEmpty(self->absolutePath)) {
        return NULL;
    }

    lastDelimiter = strrchr(self->absolutePath->data, PATH_DELIMITER);

    if (lastDelimiter == NULL) {
        result = newCharStringWithCString(self->absolutePath->data);
    } else {
        result = newCharStringWithCString(lastDelimiter + 1);
    }

    return result;
}

File fileGetParent(File self) {
    File result = NULL;
    CharString path = NULL;
    char *lastDelimiter = NULL;

    if (self->absolutePath == NULL || charStringIsEmpty(self->absolutePath)) {
        return NULL;
    }

    lastDelimiter = strrchr(self->absolutePath->data, PATH_DELIMITER);

    if (lastDelimiter == NULL) {
        result = newFileWithPath(self->absolutePath);
    } else {
        path = newCharStringWithCapacity(kFileNameStringLength);
        strncpy(path->data, self->absolutePath->data, lastDelimiter - self->absolutePath->data);
        result = newFileWithPath(path);
        freeCharString(path);
    }

    return result;
}

CharString fileGetExtension(File self) {
    CharString basename = fileGetBasename(self);
    CharString result = NULL;
    char *dot = NULL;

    if (basename == NULL || charStringIsEmpty(basename)) {
        freeCharString(basename);
        return NULL;
    }

    dot = strrchr(basename->data, '.');

    if (dot != NULL) {
        result = newCharStringWithCString(dot + 1);
    }

    freeCharString(basename);
    return result;
}

CharString fileGetExecutablePath(void) {
    CharString executablePath = newCharString();
#if LINUX
    ssize_t result = readlink("/proc/self/exe", executablePath->data, executablePath->capacity);

    if (result < 0) {
        logWarn("Could not find executable path, %s", stringForLastError(errno));
        return NULL;
    }

#elif MACOSX
    _NSGetExecutablePath(executablePath->data, (uint32_t *)&executablePath->capacity);
#elif WINDOWS
    GetModuleFileNameA(NULL, executablePath->data, (DWORD)executablePath->capacity);
#endif
    return executablePath;
}

CharString fileGetCurrentDirectory(void) {
    CharString currentDirectory = newCharString();
#if UNIX

    if (getcwd(currentDirectory->data, currentDirectory->capacity) == NULL) {
        logError("Could not get current working directory");
        freeCharString(currentDirectory);
        return NULL;
    }

#elif WINDOWS
    GetCurrentDirectoryA((DWORD)currentDirectory->capacity, currentDirectory->data);
#endif
    return currentDirectory;
}

void fileClose(File self) {
    if (self->_fileHandle != NULL && self->fileType == kFileTypeFile) {
        fflush(self->_fileHandle);
        fclose(self->_fileHandle);
        self->_fileHandle = NULL;
        self->_openMode = kFileOpenModeClosed;
    }
}

void freeFile(File self) {
    if (self) {
        fileClose(self);
        freeCharString(self->absolutePath);
        free(self);
    }
}
