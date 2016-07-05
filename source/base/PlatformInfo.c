//
// PlatformInfo.c - MrsWatson
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

#include "PlatformInfo.h"

#include "logging/EventLogger.h"

#include <stdio.h>
#include <string.h>

#if LINUX
#include "base/File.h"
#include <sys/utsname.h>

#define LSB_FILE_PATH "/etc/lsb-release"
#define LSB_DISTRIBUTION "DISTRIB_DESCRIPTION"
#endif

static PlatformType _getPlatformType() {
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

static const char *_getShortPlatformName(void) {
#if MACOSX
  return "Mac OS X";
#elif WINDOWS

  if (platformInfoIsRuntime64Bit()) {
    return "Windows 64-bit";
  } else {
    return "Windows 32-bit";
  }

#elif LINUX

  if (platformInfoIsRuntime64Bit()) {
    return "Linux-x86_64";
  } else {
    return "Linux-i686";
  }

#else
  return "Unsupported";
#endif
}

#if LINUX
static void _findLsbDistribution(void *item, void *userData) {
  CharString line = (CharString)item;
  CharString distributionName = (CharString)userData;
  LinkedList tokens = charStringSplit(line, '=');

  if (tokens != NULL && linkedListLength(tokens) == 2) {
    CharString *tokensArray = (CharString *)linkedListToArray(tokens);
    CharString key = tokensArray[0];
    CharString value = tokensArray[1];

    if (!strcmp(key->data, LSB_DISTRIBUTION)) {
      charStringCopy(distributionName, value);
    }

    free(tokensArray);
  }

  freeLinkedListAndItems(tokens, (LinkedListFreeItemFunc)freeCharString);
}
#endif

#if MACOSX
extern void _getMacVersionString(CharString outString);
#endif

static CharString _getPlatformName(void) {
  CharString result = newCharString();
#if MACOSX
  charStringCopyCString(result, _getShortPlatformName());
  _getMacVersionString(result);
#elif LINUX
  CharString distributionName = newCharString();
  struct utsname systemInfo;
  File lsbRelease = NULL;
  LinkedList lsbReleaseLines = NULL;

  if (uname(&systemInfo) != 0) {
    logWarn("Could not get system information from uname");
    charStringCopyCString(result, "Linux (Unknown platform)");
    freeCharString(distributionName);
    return result;
  }

  charStringCopyCString(distributionName, "(Unknown distribution)");

  lsbRelease = newFileWithPathCString(LSB_FILE_PATH);

  if (fileExists(lsbRelease)) {
    lsbReleaseLines = fileReadLines(lsbRelease);

    if (lsbReleaseLines != NULL && linkedListLength(lsbReleaseLines) > 0) {
      linkedListForeach(lsbReleaseLines, _findLsbDistribution,
                        distributionName);
    }
  }

  if (charStringIsEmpty(result)) {
    snprintf(result->data, result->capacity, "Linux %s, kernel %s %s",
             distributionName->data, systemInfo.release, systemInfo.machine);
  }

  freeCharString(distributionName);
  freeLinkedListAndItems(lsbReleaseLines,
                         (LinkedListFreeItemFunc)freeCharString);
  freeFile(lsbRelease);
#elif WINDOWS
  OSVERSIONINFOEX versionInformation;
  memset(&versionInformation, 0, sizeof(OSVERSIONINFOEX));
  versionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  GetVersionEx((OSVERSIONINFO *)&versionInformation);
  // Generic string which will also work with newer versions of windows
  snprintf(result->data, result->capacity, "Windows %d.%d",
           versionInformation.dwMajorVersion,
           versionInformation.dwMinorVersion);

  // This is a bit lame, but it seems that this is the standard way of getting
  // the platform name on Windows.
  switch (versionInformation.dwMajorVersion) {
  case 6:
    switch (versionInformation.dwMinorVersion) {
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
    switch (versionInformation.dwMinorVersion) {
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

boolByte platformInfoIsRuntime64Bit(void) {
  return (boolByte)(sizeof(void *) == 8);
}

boolByte platformInfoIsHost64Bit(void) {
  boolByte result = false;

#if LINUX
  struct utsname systemInfo;

  if (uname(&systemInfo) != 0) {
    logError("Could not get system bitness from uname");
  } else {
    result = (boolByte)(strcmp(systemInfo.machine, "x86_64") == 0);
  }

#elif MACOSX
#elif WINDOWS
  typedef BOOL(WINAPI * IsWow64ProcessFuncPtr)(HANDLE, PBOOL);
  BOOL isProcessRunningInWow64 = false;
  IsWow64ProcessFuncPtr isWow64ProcessFunc = NULL;

  // The IsWow64Process() function is not available on all versions of Windows,
  // so it must be looked up first and called only if it exists.
  isWow64ProcessFunc = (IsWow64ProcessFuncPtr)GetProcAddress(
      GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

  if (isWow64ProcessFunc != NULL) {
    if (isWow64ProcessFunc(GetCurrentProcess(), &isProcessRunningInWow64)) {
      // IsWow64Process will only return true if the current process is a 32-bit
      // application running on 64-bit Windows.
      if (isProcessRunningInWow64) {
        result = true;
      } else {
        // If false, then we can assume that the host has the same bitness as
        // the executable.
        result = platformInfoIsRuntime64Bit();
      }
    }
  }

#else
  logUnsupportedFeature("Get host 64-bitness");
#endif

  return result;
}

boolByte platformInfoIsLittleEndian(void) {
  int num = 1;
  return (boolByte)(*(char *)&num == 1);
}

PlatformInfo newPlatformInfo(void) {
  PlatformInfo platformInfo = (PlatformInfo)malloc(sizeof(PlatformInfoMembers));
  platformInfo->type = _getPlatformType();
  platformInfo->name = _getPlatformName();
  platformInfo->shortName = newCharStringWithCString(_getShortPlatformName());
  platformInfo->is64Bit = platformInfoIsHost64Bit();
  return platformInfo;
}

void freePlatformInfo(PlatformInfo self) {
  if (self != NULL) {
    freeCharString(self->name);
    freeCharString(self->shortName);
    free(self);
  }
}
