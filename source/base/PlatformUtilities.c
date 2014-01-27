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
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "base/File.h"
#include "base/PlatformUtilities.h"
#include "logging/EventLogger.h"

#if WINDOWS
#include <Windows.h>
#elif UNIX
#include <dirent.h>
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

PlatformType getPlatformType() {
#if MACOSX
  return PLATFORM_MACOSX;
#elif WINDOWS
  return PLATFORM_WINDOWS;
#elif LINUX
  return PLATFORM_LINUX;
#else
  return PLATFORM_UNSUPPORTED;
#endif
}

const char* getShortPlatformName(void) {
#if MACOSX
  return "Mac OS X";
#elif WINDOWS
  if(isExecutable64Bit()) {
    return "Windows 64-bit";
  }
  else {
    return "Windows 32-bit";
  }
#elif LINUX
  if(isExecutable64Bit()) {
    return "Linux-x86_64";
  }
  else {
    return "Linux-i686";
  }
#else
  return "Unsupported";
#endif
}

#if LINUX
void _findLsbDistribution(void* item, void* userData) {
  CharString line = (CharString)item;
  CharString distributionName = (CharString)userData;
  LinkedList tokens = charStringSplit(line, '=');
  if(tokens != NULL && linkedListLength(tokens) == 2) {
    CharString* tokensArray = (CharString*)linkedListToArray(tokens);
    CharString key = tokensArray[0];
    CharString value = tokensArray[1];
    if(!strcmp(key->data, LSB_DISTRIBUTION)) {
      charStringCopy(distributionName, value);
    }
    free(tokensArray);
  }
  freeLinkedListAndItems(tokens, (LinkedListFreeItemFunc)freeCharString);
}
#endif

CharString getPlatformName(void) {
  CharString result = newCharString();
#if MACOSX
  SInt32 major, minor, bugfix;
  Gestalt(gestaltSystemVersionMajor, &major);
  Gestalt(gestaltSystemVersionMinor, &minor);
  Gestalt(gestaltSystemVersionBugFix, &bugfix);
  snprintf(result->data, result->capacity, "Mac OS X %d.%d.%d", (int)major, (int)minor, (int)bugfix);
#elif LINUX
  CharString distributionName = newCharString();
  struct utsname systemInfo;
  File lsbRelease = NULL;
  LinkedList lsbReleaseLines = NULL;

  if(uname(&systemInfo) != 0) {
    logWarn("Could not get system information from uname");
    charStringCopyCString(result, "Linux (Unknown platform)");
    freeCharString(distributionName);
    return result;
  }
  charStringCopyCString(distributionName, "(Unknown distribution)");

  lsbRelease = newFileWithPathCString(LSB_FILE_PATH);
  if(fileExists(lsbRelease)) {
    lsbReleaseLines = fileReadLines(lsbRelease);
    if(lsbReleaseLines != NULL && linkedListLength(lsbReleaseLines) > 0) {
      linkedListForeach(lsbReleaseLines, _findLsbDistribution, distributionName);
    }
  }

  if(charStringIsEmpty(result)) {
    snprintf(result->data, result->capacity, "Linux %s, kernel %s %s",
      distributionName->data, systemInfo.release, systemInfo.machine);
  }

  freeCharString(distributionName);
  freeLinkedListAndItems(lsbReleaseLines, (LinkedListFreeItemFunc)freeCharString);
  freeFile(lsbRelease);
#elif WINDOWS
  OSVERSIONINFOEX versionInformation;
  memset(&versionInformation, 0, sizeof(OSVERSIONINFOEX));
  versionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  GetVersionEx((OSVERSIONINFO*)&versionInformation);
  // Generic string which will also work with newer versions of windows
  snprintf(result->data, result->capacity, "Windows %d.%d",
    versionInformation.dwMajorVersion, versionInformation.dwMinorVersion);

  // This is a bit lame, but it seems that this is the standard way of getting
  // the platform name on Windows.
  switch(versionInformation.dwMajorVersion) {
    case 6:
      switch(versionInformation.dwMinorVersion) {
      case 2:
        charStringCopyCString(result, "Windows 8");
        break;
      case 1:
        charStringCopyCString(result, "Windows 7");
        break;
      case 0:
        charStringCopyCString(result, "Windows Vista");
        break;
      }
      break;
    case 5:
      switch(versionInformation.dwMinorVersion) {
      case 2:
        charStringCopyCString(result, "Windows Server 2003");
        break;
      case 1:
        charStringCopyCString(result, "Windows XP");
        break;
      case 0:
        charStringCopyCString(result, "Windows 2000");
        break;
      }
      break;
  }
#else
  charStringCopyCString(result, "Unsupported platform");
#endif
  return result;
}

CharString getExecutablePath(void) {
  CharString executablePath = newCharString();
#if LINUX
  ssize_t result = readlink("/proc/self/exe", executablePath->data, executablePath->capacity);
  if(result < 0) {
    logWarn("Could not find executable path, %s", stringForLastError(errno));
    return NULL;
  }
#elif MACOSX
  _NSGetExecutablePath(executablePath->data, (uint32_t*)&executablePath->capacity);
#elif WINDOWS
  GetModuleFileNameA(NULL, executablePath->data, (DWORD)executablePath->capacity);
#endif
  return executablePath;
}

CharString getCurrentDirectory(void) {
  CharString currentDirectory = newCharString();
#if UNIX
  if(getcwd(currentDirectory->data, currentDirectory->capacity) == NULL) {
    logError("Could not get current working directory");
    freeCharString(currentDirectory);
    return NULL;
  }
#elif WINDOWS
  GetCurrentDirectoryA((DWORD)currentDirectory->capacity, currentDirectory->data);
#endif
  return currentDirectory;
}

boolByte isExecutable64Bit(void) {
  return (sizeof(void*) == 8);
}

boolByte isHost64Bit(void) {
  boolByte result = false;

#if LINUX
  struct utsname systemInfo;
  if(uname(&systemInfo) != 0) {
    logError("Could not get system bitness from uname");
  }
  else {
    result = (strcmp(systemInfo.machine, "x86_64") == 0);
  }
#elif MACOSX
#elif WINDOWS
  typedef BOOL (WINAPI *IsWow64ProcessFuncPtr)(HANDLE, PBOOL);
  BOOL isProcessRunningInWow64 = false;
  IsWow64ProcessFuncPtr isWow64ProcessFunc = NULL;

  // The IsWow64Process() function is not available on all versions of Windows,
  // so it must be looked up first and called only if it exists.
  isWow64ProcessFunc = (IsWow64ProcessFuncPtr)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
  if(isWow64ProcessFunc != NULL) {
    if(isWow64ProcessFunc(GetCurrentProcess(), &isProcessRunningInWow64)) {
      // IsWow64Process will only return true if the current process is a 32-bit
      // application running on 64-bit Windows.
      if(isProcessRunningInWow64) {
        result = true;
      }
      else {
        // If false, then we can assume that the host has the same bitness as
        // the executable.
        result = isExecutable64Bit();
      }
    }
  }
#else
  logUnsupportedFeature("Get host 64-bitness");
#endif

  return result;
}

boolByte isHostLittleEndian(void) {
  int num = 1;
  boolByte result = (*(char*)&num == 1);
  return result;
}

unsigned short flipShortEndian(const unsigned short value) {
  return (value << 8) | (value >> 8);
}

unsigned short convertBigEndianShortToPlatform(const unsigned short value) {
  if(isHostLittleEndian()) {
    return (value << 8) | (value >> 8);
  }
  else {
    return value;
  }
}

unsigned int convertBigEndianIntToPlatform(const unsigned int value) {
  if(isHostLittleEndian()) {
    return (value << 24) | ((value << 8) & 0x00ff0000) | ((value >> 8) & 0x0000ff00) | (value >> 24);
  }
  else {
    return value;
  }
}

unsigned int convertLittleEndianIntToPlatform(const unsigned int value) {
  if(!isHostLittleEndian()) {
    return (value << 24) | ((value << 8) & 0x00ff0000) | ((value >> 8) & 0x0000ff00) | (value >> 24);
  }
  else {
    return value;
  }
}

unsigned short convertByteArrayToUnsignedShort(const byte* value) {
  if(isHostLittleEndian()) {
    return ((value[1] << 8) & 0x0000ff00) | value[0];
  }
  else {
    return ((value[0] << 8) & 0x0000ff00) | value[1];
  }
}

unsigned int convertByteArrayToUnsignedInt(const byte* value) {
  if(isHostLittleEndian()) {
    return ((value[3] << 24) | ((value[2] << 16) & 0x00ff0000) |
      ((value[1] << 8) & 0x0000ff00) | value[0]);
  }
  else {
    return ((value[0] << 24) | ((value[1] << 16) & 0x00ff0000) |
      ((value[2] << 8) & 0x0000ff00) | value[0]);
  }
}

float convertBigEndianFloatToPlatform(const float value) {
  float result = 0.0f;
  byte* floatToConvert = (byte*)&value;
  byte* floatResult = (byte*)&result;
  floatResult[0] = floatToConvert[3];
  floatResult[1] = floatToConvert[2];
  floatResult[2] = floatToConvert[1];
  floatResult[3] = floatToConvert[0];
  return result;
}

void sleepMilliseconds(const double milliseconds) {
#if UNIX
  struct timespec sleepTime;
  sleepTime.tv_sec = 0;
  sleepTime.tv_nsec = (long)(1000000.0 * milliseconds);
  nanosleep(&sleepTime, NULL);
#elif WINDOWS
  Sleep((DWORD)milliseconds);
#endif
}
