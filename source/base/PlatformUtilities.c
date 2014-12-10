//
// PlatformUtilities.c - MrsWatson
// Created by Nik Reiman on 1/2/12.
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

#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

#include "base/File.h"
#include "base/PlatformInfo.h"
#include "base/PlatformUtilities.h"
#include "logging/EventLogger.h"

#if WINDOWS
#include <Windows.h>
#elif UNIX
#include <unistd.h>
#if MACOSX
#include <CoreServices/CoreServices.h>
#include <mach-o/dyld.h>
#elif LINUX
#define LSB_FILE_PATH "/etc/lsb-release"
#define LSB_DISTRIBUTION "DISTRIB_DESCRIPTION"
#include <sys/utsname.h>
#include <errno.h>
#endif
#endif

CharString getExecutablePath(void)
{
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

CharString getCurrentDirectory(void)
{
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

boolByte isExecutable64Bit(void)
{
    return (boolByte)(sizeof(void *) == 8);
}

unsigned short flipShortEndian(const unsigned short value)
{
    return (value << 8) | (value >> 8);
}

unsigned short convertBigEndianShortToPlatform(const unsigned short value)
{
    if (platformInfoIsLittleEndian()) {
        return (value << 8) | (value >> 8);
    } else {
        return value;
    }
}

unsigned int convertBigEndianIntToPlatform(const unsigned int value)
{
    if (platformInfoIsLittleEndian()) {
        return (value << 24) | ((value << 8) & 0x00ff0000) | ((value >> 8) & 0x0000ff00) | (value >> 24);
    } else {
        return value;
    }
}

unsigned int convertLittleEndianIntToPlatform(const unsigned int value)
{
    if (!platformInfoIsLittleEndian()) {
        return (value << 24) | ((value << 8) & 0x00ff0000) | ((value >> 8) & 0x0000ff00) | (value >> 24);
    } else {
        return value;
    }
}

unsigned short convertByteArrayToUnsignedShort(const byte *value)
{
    if (platformInfoIsLittleEndian()) {
        return ((value[1] << 8) & 0x0000ff00) | value[0];
    } else {
        return ((value[0] << 8) & 0x0000ff00) | value[1];
    }
}

unsigned int convertByteArrayToUnsignedInt(const byte *value)
{
    if (platformInfoIsLittleEndian()) {
        return ((value[3] << 24) | ((value[2] << 16) & 0x00ff0000) |
                ((value[1] << 8) & 0x0000ff00) | value[0]);
    } else {
        return ((value[0] << 24) | ((value[1] << 16) & 0x00ff0000) |
                ((value[2] << 8) & 0x0000ff00) | value[0]);
    }
}

float convertBigEndianFloatToPlatform(const float value)
{
    float result = 0.0f;
    byte *floatToConvert = (byte *)&value;
    byte *floatResult = (byte *)&result;
    floatResult[0] = floatToConvert[3];
    floatResult[1] = floatToConvert[2];
    floatResult[2] = floatToConvert[1];
    floatResult[3] = floatToConvert[0];
    return result;
}

void sleepMilliseconds(const double milliseconds)
{
#if UNIX
    struct timespec sleepTime;
    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = (long)(1000000.0 * milliseconds);
    nanosleep(&sleepTime, NULL);
#elif WINDOWS
    Sleep((DWORD)milliseconds);
#endif
}
