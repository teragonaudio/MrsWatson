//
//  CharStringList.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "CharString.h"

#ifndef MrsWatson_CharStringList_h
#define MrsWatson_CharStringList_h

typedef struct {
  CharString item;
  void* nextItem;
} CharStringListMembers;

typedef CharStringListMembers* CharStringList;
typedef CharStringListMembers*CharStringListIterator;

CharStringList newCharStringList(void);
void appendItemToStringList(CharStringList list, const CharString charString);
int numItemsInStringList(CharStringList list);
void freeCharStringList(CharStringList list);

#endif
