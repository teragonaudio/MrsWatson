//
//  StringUtilities.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/4/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#ifndef MrsWatson_StringUtilities_h
#define MrsWatson_StringUtilities_h

#define TERMINAL_LINE_LENGTH 80

char* getFileBasename(const char* filename);
char* getFileExtension(const char* filename);

void wrapString(const char* srcString, char* destString, int indentSize, int lineLength);
void wrapStringForTerminal(const char* srcString, char* destString, int indentSize);

#endif
