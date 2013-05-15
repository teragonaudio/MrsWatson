//
// PluginVst2xWindows.c - MrsWatson
// Created by Nik Reiman on 13 May 13.
// Copyright (c) 2013 Teragon Audio. All rights reserved.
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

#if WINDOWS
#define VST_FORCE_DEPRECATED 0
#include "aeffectx.h"
#include "plugin/PluginVst2xHostCallback.h"

extern "C" {
#include <stdio.h>
#include "base/PlatformUtilities.h"
#include "logging/EventLogger.h"

typedef BOOL (WINAPI *IsWow64ProcessFuncPtr)(HANDLE, PBOOL);

static const char* kPlatformWindowsProgramFolder = "C:\\Program Files";
static const char* kPlatformWindows32BitProgramFolder = "C:\\Program Files (x86)";

boolByte is32BitExeOn64BitHost() {
  boolByte isWindows64 = false;
  IsWow64ProcessFuncPtr isWow64ProcessFunc = NULL;

  // The IsWow64Process() function is not available on all versions of Windows,
  // so it must be looked up first and called only if it exists.
  isWow64ProcessFunc = (IsWow64ProcessFuncPtr)GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
  if(isWow64ProcessFunc != NULL) {
    if(isWow64ProcessFunc(GetCurrentProcess(), &isWindows64)) {
      return true;
    }
  }

  return false;
}

LinkedList getVst2xPluginLocations(CharString currentDirectory) {
  LinkedList locations = newLinkedList();
  CharString locationBuffer;
  char* programFiles = (isHost64Bit() && is32BitExeOn64BitHost()) ?
    kPlatformWindows32BitProgramFolder : kPlatformWindowsProgramFolder;

  appendItemToList(locations, currentDirectory);

  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->length), "C:\\VstPlugins");
  appendItemToList(locations, locationBuffer);

  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->length), "%s\\Common Files\\VstPlugins", programFiles);
  appendItemToList(locations, locationBuffer);

  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->length), "%s\\Steinberg\\VstPlugins", programFiles);
  appendItemToList(locations, locationBuffer);

  return locations;
}

LibraryHandle getLibraryHandleForPlugin(const CharString pluginAbsolutePath) {
  HMODULE libraryHandle = LoadLibraryExA((LPCSTR)pluginAbsolutePath->data, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
  if(libraryHandle == NULL) {
    logError("Could not open library, error code '%d'", GetLastError());
    return NULL;
  }
  return libraryHandle;
}

AEffect* loadVst2xPlugin(LibraryHandle libraryHandle) {
  Vst2xPluginEntryFunc entryPoint = (Vst2xPluginEntryFunc)GetProcAddress(libraryHandle, "VSTPluginMain");

  if(entryPoint == NULL) {
    entryPoint = (Vst2xPluginEntryFunc)GetProcAddress(libraryHandle, "VstPluginMain()"); 
  }

  if(entryPoint == NULL) {
    entryPoint = (Vst2xPluginEntryFunc)GetProcAddress(libraryHandle, "main");
  }

  if(entryPoint == NULL) {
    logError("Couldn't get a pointer to plugin's main()");
    return NULL;
  }

  AEffect* plugin = entryPoint(pluginVst2xHostCallback);
  return plugin;
}

void closeLibraryHandle(LibraryHandle libraryHandle) {
  FreeLibrary(libraryHandle);
}

} // extern "C"
#endif
