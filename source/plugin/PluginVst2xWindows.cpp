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

#include "base/PlatformUtilities.h"

LinkedList getVst2xPluginLocations(CharString currentDirectory) {
  LinkedList locations = newLinkedList();
  CharString locationBuffer;

  appendItemToList(locations, currentPath);

  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->length), "C:\\VstPlugins");
  appendItemToList(outLocations, locationBuffer1);

  // TODO: On a 64-bit windows, this should be c:\program files (x86) if the host is 32-bit
  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->length), "C:\\Program Files\\Common Files\\VstPlugins");
  appendItemToList(outLocations, locationBuffer);

  // TODO: On a 64-bit windows, this should be c:\program files (x86) if the host is 32-bit
  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->length), "C:\\Program Files\\Steinberg\\VstPlugins");
  appendItemToList(outLocations, locationBuffer);

  return locations;
}

LibraryHandle _moduleHandleForPlugin(const CharString pluginAbsolutePath) {
  HMODULE moduleHandle = LoadLibraryExA((LPCSTR)pluginAbsolutePath->data, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
  if(moduleHandle == NULL) {
    logError("Could not open library, error code '%d'", GetLastError());
    return NULL;
  }
  return moduleHandle;
}

AEffect* loadVst2xPlugin(LibraryHandle moduleHandle) {
  Vst2xPluginEntryFunc entryPoint = (Vst2xPluginEntryFunc)GetProcAddress(moduleHandle, "VSTPluginMain");

  if(entryPoint == NULL) {
    entryPoint = (Vst2xPluginEntryFunc)GetProcAddress(moduleHandle, "VstPluginMain()"); 
  }

  if(entryPoint == NULL) {
    entryPoint = (Vst2xPluginEntryFunc)GetProcAddress(moduleHandle, "main");
  }

  if(entryPoint == NULL) {
    logError("Couldn't get a pointer to plugin's main()");
    return NULL;
  }

  AEffect* plugin = entryPoint(vst2xPluginHostCallback);
  return plugin;
}

// For closing:
//   FreeLibrary(data->moduleHandle);


#endif
