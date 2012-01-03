//
//  PlatformInfo.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#import <stdlib.h>
#include <sys/stat.h>
#import "PlatformInfo.h"

PlatformType getPlatformType() {
#if MACOSX
  return PLATFORM_MACOSX;
#elif WIN32
  return PLATFORM_WINDOWS;
#else
  return PLATFORM_UNSUPPORTED;
#endif
}

boolean fileExists(const CharString absolutePath) {
#if MACOSX
  struct stat* buffer = malloc(sizeof(struct stat));
  boolean result = (stat(absolutePath, buffer) == 0);
  free(buffer);
  return result;
#elif WIN32
  // TODO: this
  return false;
#else
  return false;
#endif
}
