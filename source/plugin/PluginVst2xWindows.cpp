//
// PluginVst2xWindows.c - MrsWatson
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

#if WINDOWS
#include "PluginVst2xHostCallback.h"

#define VST_FORCE_DEPRECATED 0
#include "aeffectx.h"

extern "C" {
#include "base/PlatformInfo.h"
#include "logging/EventLogger.h"
#include "plugin/Plugin.h"
#include <stdio.h>

#include <stdio.h>

static const char *kPlatformWindowsProgramFolder = "C:\\Program Files";
static const char *kPlatformWindows32BitProgramFolder =
    "C:\\Program Files (x86)";

LinkedList getVst2xPluginLocations(CharString currentDirectory) {
  LinkedList locations = newLinkedList();
  CharString locationBuffer;
  const char *programFiles =
      (!platformInfoIsRuntime64Bit() && platformInfoIsHost64Bit())
          ? kPlatformWindows32BitProgramFolder
          : kPlatformWindowsProgramFolder;

  linkedListAppend(locations, currentDirectory);

  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->capacity),
           "C:\\VstPlugins");
  linkedListAppend(locations, locationBuffer);

  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->capacity),
           "%s\\VstPlugIns", programFiles);
  linkedListAppend(locations, locationBuffer);

  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->capacity),
           "%s\\Common Files\\VstPlugIns", programFiles);
  linkedListAppend(locations, locationBuffer);

  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->capacity),
           "%s\\Steinberg\\VstPlugIns", programFiles);
  linkedListAppend(locations, locationBuffer);

  return locations;
}

LibraryHandle getLibraryHandleForPlugin(const CharString pluginAbsolutePath) {
  HMODULE libraryHandle = LoadLibraryExA((LPCSTR)pluginAbsolutePath->data, NULL,
                                         LOAD_WITH_ALTERED_SEARCH_PATH);
  DWORD errorCode = GetLastError();

  if (libraryHandle == NULL) {
    if (errorCode == ERROR_BAD_EXE_FORMAT) {
      logError("Could not open library, wrong architecture");
    } else {
      logError("Could not open library, error code %d (%s)", errorCode,
               stringForLastError(errorCode));
    }

    return NULL;
  }

  return libraryHandle;
}

AEffect *loadVst2xPlugin(LibraryHandle libraryHandle) {
  Vst2xPluginEntryFunc entryPoint =
      (Vst2xPluginEntryFunc)GetProcAddress(libraryHandle, "VSTPluginMain");

  if (entryPoint == NULL) {
    entryPoint =
        (Vst2xPluginEntryFunc)GetProcAddress(libraryHandle, "VstPluginMain()");
  }

  if (entryPoint == NULL) {
    entryPoint = (Vst2xPluginEntryFunc)GetProcAddress(libraryHandle, "main");
  }

  if (entryPoint == NULL) {
    logError("Couldn't get a pointer to plugin's main()");
    return NULL;
  }

  AEffect *plugin = entryPoint(pluginVst2xHostCallback);
  return plugin;
}

void showVst2xEditor(AEffect *effect, const CharString pluginName,
                     PluginWindowSize *rect) {
  logUnsupportedFeature("Show VST editor on Windows");
}

void closeLibraryHandle(LibraryHandle libraryHandle) {
  FreeLibrary(libraryHandle);
}

} // extern "C"
#endif
