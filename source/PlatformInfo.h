//
//  PlatformInfo.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#import "Types.h"
#import "CharString.h"

#ifndef MrsWatson_PlatformInfo_h
#define MrsWatson_PlatformInfo_h

#if WIN32
#define PATH_DELIMITER '\\'
#else
#define PATH_DELIMITER '/'
#endif

typedef enum {
  PLATFORM_UNSUPPORTED,
  PLATFORM_MACOSX,
  PLATFORM_WINDOWS
} PlatformType;

PlatformType getPlatformType(void);
boolean fileExists(const CharString absolutePath);

#endif
