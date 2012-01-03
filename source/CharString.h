//
//  CharString.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#import "Types.h"

#ifndef MrsWatson_CharString_h
#define MrsWatson_CharString_h

#define STRING_LENGTH 256
#define STRING_LENGTH_SHORT 32
#define STRING_LENGTH_LONG 8192

#define TERMINAL_LINE_LENGTH 84

typedef char* CharString;

CharString newCharString(void);
CharString newCharStringShort(void);
CharString newCharStringLong(void);

boolean isStringEmpty(CharString testString);

void wrapCharString(const CharString srcString, CharString destString, int indentSize, int lineLength);
void wrapCharStringForTerminal(const CharString srcString, CharString destString, int indentSize);

CharString getFileBasename(const CharString filename);
CharString getFileExtension(const CharString filename);

#endif
