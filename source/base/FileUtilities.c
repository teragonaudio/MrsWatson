//
// FileUtilities.c - MrsWatson
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

#include "base/FileUtilities.h"

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
#include <dirent.h>
#endif

/** DEPRECATED */
void buildAbsolutePath(const CharString directory, const CharString file, const char *fileExtension, CharString outString)
{
    const char *extension;
    CharString absoluteDirectory;

    if (directory == NULL || charStringIsEmpty(directory)) {
        logWarn("Attempt to build absolute path with empty directory");
        return;
    }

    if (file == NULL || charStringIsEmpty(file)) {
        logWarn("Attempt to build absolute path with empty file");
        return;
    }

    absoluteDirectory = newCharString();

    if (isAbsolutePath(directory)) {
        charStringCopy(absoluteDirectory, directory);
    } else {
        convertRelativePathToAbsolute(directory, absoluteDirectory);
    }

    if (fileExtension != NULL) {
        // Ignore attempts to append the same extension as is already on the file
        extension = getFileExtension(file->data);

        if (extension != NULL && !strncasecmp(extension, fileExtension, strlen(extension))) {
            buildAbsolutePath(absoluteDirectory, file, NULL, outString);
        } else {
            snprintf(outString->data, outString->capacity, "%s%c%s.%s",
                     absoluteDirectory->data, PATH_DELIMITER, file->data, fileExtension);
        }
    } else {
        snprintf(outString->data, outString->capacity, "%s%c%s",
                 absoluteDirectory->data, PATH_DELIMITER, file->data);
    }

    freeCharString(absoluteDirectory);
}

/** DEPRECATED */
void convertRelativePathToAbsolute(const CharString file, CharString outString)
{
    CharString currentDirectory = fileGetCurrentDirectory();
    snprintf(outString->data, outString->capacity, "%s%c%s", currentDirectory->data, PATH_DELIMITER, file->data);
    freeCharString(currentDirectory);
}

/** DEPRECATED */
boolByte isAbsolutePath(const CharString path)
{
#if WINDOWS

    if (path->capacity > 3) {
        // Check for strings which start with a drive letter, ie C:\file
        if (path->data[1] == ':' && path->data[2] == PATH_DELIMITER) {
            return true;
        }
        // Check for network paths, ie \\SERVER\file
        else if (path->data[0] == PATH_DELIMITER && path->data[1] == PATH_DELIMITER) {
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

/** DEPRECATED */
const char *getFileBasename(const char *filename)
{
    const char *lastDelimiter;

    if (filename == NULL) {
        return NULL;
    }

    lastDelimiter = strrchr(filename, PATH_DELIMITER);

    if (lastDelimiter == NULL) {
        return (char *)filename;
    } else {
        return lastDelimiter + 1;
    }
}

/** DEPRECATED */
const char *getFileExtension(const char *filename)
{
    const char *dot;

    if (filename == NULL) {
        return NULL;
    }

    dot = strrchr(filename, '.');

    if (dot == NULL) {
        return NULL;
    } else {
        return dot + 1;
    }
}
