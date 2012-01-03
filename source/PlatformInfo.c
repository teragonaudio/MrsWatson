//
//  PlatformInfo.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
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

