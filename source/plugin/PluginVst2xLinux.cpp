//
// PluginVst2xLinux.c - MrsWatson
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

#if LINUX
#define VST_FORCE_DEPRECATED 0
#include "aeffectx.h"

extern "C" {
#include <dlfcn.h>
#include <stdlib.h>
#include "base/CharString.h"
#include "base/LinkedList.h"
#include "base/PlatformUtilities.h"
#include "logging/EventLogger.h"
#include "plugin/PluginVst2xCallbacks.h"

LinkedList getVst2xPluginLocations(CharString currentDirectory) {
  LinkedList locations = newLinkedList();
  CharString locationBuffer;
  char* vstPathEnv;

  appendItemToList(locations, currentDirectory);

  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->length), "%s/.vst", getenv("HOME"));
  appendItemToList(locations, locationBuffer);

  locationBuffer = newCharString();
  vstPathEnv = getenv("VST_PATH");
  if(vstPathEnv != NULL) {
    snprintf(locationBuffer->data, (size_t)(locationBuffer->length), "%s", vstPathEnv);
    appendItemToList(locations, locationBuffer);
  }

  return locations;
}

LibraryHandle getLibraryHandleForPlugin(const CharString pluginAbsolutePath) {
  void* libraryHandle = dlopen(pluginAbsolutePath->data, RTLD_NOW | RTLD_LOCAL);
  if(libraryHandle == NULL) {
    logError("Could not open library, %s", dlerror());
    return NULL;
  }
  return libraryHandle;
}

AEffect* loadVst2xPlugin(LibraryHandle libraryHandle) {
  // Somewhat cheap hack to avoid a tricky compiler warning. Casting from void* to a proper function
  // pointer will cause GCC to warn that "ISO C++ forbids casting between pointer-to-function and
  // pointer-to-object". Here, we represent both types in a union and use the correct one in the given
  // context, thus avoiding the need to cast anything.
  // See also: http://stackoverflow.com/a/2742234/14302
  union {
    Vst2xPluginEntryFunc entryPointFuncPtr;
    void *entryPointVoidPtr;
  } entryPoint;

  entryPoint.entryPointVoidPtr = dlsym(libraryHandle, "VSTPluginMain");
  if(entryPoint.entryPointVoidPtr == NULL) {
    entryPoint.entryPointVoidPtr = dlsym(libraryHandle, "main");
    if(entryPoint.entryPointVoidPtr == NULL) {
      logError("Couldn't get a pointer to plugin's main()");
      return NULL;
    }
  }
  Vst2xPluginEntryFunc mainEntryPoint = entryPoint.entryPointFuncPtr;
  AEffect* plugin = mainEntryPoint(vst2xPluginHostCallback);
  return plugin;
}

void closeLibraryHandle(LibraryHandle libraryHandle) {
  dlclose(libraryHandle);
}

} // extern "C"
#endif
