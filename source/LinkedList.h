//
//  LinkedList.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "CharString.h"

#ifndef MrsWatson_CharStringList_h
#define MrsWatson_CharStringList_h

typedef struct {
  void* item;
  void* nextItem;
} CharStringListMembers;

typedef CharStringListMembers* CharStringList;
typedef CharStringListMembers*CharStringListIterator;

typedef void (*CharStringListFreeFunc)(void*);

CharStringList newCharStringList(void);
void appendItemToStringList(CharStringList list, void* charString);
int numItemsInStringList(CharStringList list);
void freeCharStringList(CharStringList list, CharStringListFreeFunc freeFunc);

#endif
