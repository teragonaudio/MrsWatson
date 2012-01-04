//
//  CharStringList.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "CharStringList.h"

CharStringList newCharStringList(void) {
  CharStringList list = malloc(sizeof(CharStringListMembers));

  list->item = NULL;
  list->nextItem = NULL;

  return list;
}

void appendItemToStringList(CharStringList list, const CharString charString) {
  CharStringListIterator iterator = list;
  while(iterator->nextItem != NULL) {
    iterator = iterator->nextItem;
  }
  iterator->item = newCharString();
  copyCharStrings(iterator->item, charString);
  iterator->nextItem = newCharStringList();
}

// TODO: Currently unused, is this needed?
int numItemsInStringList(CharStringList list) {
  int result = 0;
  CharStringListIterator iterator = list;
  while(iterator != NULL) {
    result++;
    iterator = iterator->nextItem;
  }
  return result;
}

void freeCharStringList(CharStringList list) {
  CharStringListIterator iterator = list;
  while(iterator != NULL) {
    freeCharString(iterator->item);
    CharStringList current = iterator;
    iterator = iterator->nextItem;
    free(current);
  }
}
