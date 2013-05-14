//
// PluginVst2xMac.c - MrsWatson
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

#if MACOSX
#define VST_FORCE_DEPRECATED 0
#include "aeffectx.h"

extern "C" {
#include <stdlib.h>
#include <CoreFoundation/CFBundle.h>
#include "base/CharString.h"
#include "base/LinkedList.h"
#include "base/PlatformUtilities.h"
#include "logging/EventLogger.h"
#include "plugin/PluginVst2xHostCallback.h"

LinkedList getVst2xPluginLocations(CharString currentDirectory);
LinkedList getVst2xPluginLocations(CharString currentDirectory) {
  LinkedList locations = newLinkedList();
  CharString locationBuffer;

  appendItemToList(locations, currentDirectory);

  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->length), "/Library/Audio/Plug-Ins/VST");
  appendItemToList(locations, locationBuffer);

  locationBuffer = newCharString();
  snprintf(locationBuffer->data, (size_t)(locationBuffer->length), "%s/Library/Audio/Plug-Ins/VST", getenv("HOME"));
  appendItemToList(locations, locationBuffer);

  return locations;
}

LibraryHandle getLibraryHandleForPlugin(const CharString pluginAbsolutePath);
LibraryHandle getLibraryHandleForPlugin(const CharString pluginAbsolutePath) {
  // Create a path to the bundle
  CFStringRef pluginPathStringRef = CFStringCreateWithCString(NULL, pluginAbsolutePath->data, kCFStringEncodingASCII);
  CFURLRef bundleUrl = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, pluginPathStringRef, kCFURLPOSIXPathStyle, true);
  if(bundleUrl == NULL) {
    logError("Couldn't make URL reference for plugin");
    return NULL;
  }

  // Open the bundle
  CFBundleRef bundleRef = CFBundleCreate(kCFAllocatorDefault, bundleUrl);
  if(bundleRef == NULL) {
    logError("Couldn't create bundle reference");
    CFRelease(pluginPathStringRef);
    CFRelease(bundleUrl);
    return NULL;
  }

  // Clean up
  CFRelease(pluginPathStringRef);
  CFRelease(bundleUrl);

  return bundleRef;
}

AEffect* loadVst2xPlugin(LibraryHandle libraryHandle);
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

  entryPoint.entryPointVoidPtr = CFBundleGetFunctionPointerForName(libraryHandle, CFSTR("VSTPluginMain"));
  Vst2xPluginEntryFunc mainEntryPoint = entryPoint.entryPointFuncPtr;
  // VST plugins previous to the 2.4 SDK used main_macho for the entry point name
  if(mainEntryPoint == NULL) {
    entryPoint.entryPointVoidPtr = CFBundleGetFunctionPointerForName(libraryHandle, CFSTR("main_macho"));
    mainEntryPoint = entryPoint.entryPointFuncPtr;
  }

  if(mainEntryPoint == NULL) {
    logError("Couldn't get a pointer to plugin's main()");
    CFBundleUnloadExecutable(libraryHandle);
    CFRelease(libraryHandle);
    return NULL;
  }

  AEffect* plugin = mainEntryPoint(pluginVst2xHostCallback);
  if(plugin == NULL) {
    logError("Plugin's main() returns null");
    CFBundleUnloadExecutable(libraryHandle);
    CFRelease(libraryHandle);
    return NULL;
  }

  return plugin;
}

void closeLibraryHandle(LibraryHandle libraryHandle);
void closeLibraryHandle(LibraryHandle libraryHandle) {
  CFBundleUnloadExecutable(libraryHandle);
  CFRelease(libraryHandle);
}

} // extern "C"
#endif
