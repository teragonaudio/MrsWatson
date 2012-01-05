//
//  LinkedList.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"

LinkedList newLinkedList(void) {
  LinkedList list = malloc(sizeof(LinkedListMembers));

  list->item = NULL;
  list->nextItem = NULL;

  return list;
}

void appendItemToList(LinkedList list, void* item) {
  LinkedListIterator iterator = list;
  while(iterator->nextItem != NULL) {
    iterator = iterator->nextItem;
  }
  iterator->item = item;
  iterator->nextItem = newLinkedList();
}

// TODO: Currently unused, is this needed?
int numItemsInList(LinkedList list) {
  int result = 0;
  LinkedListIterator iterator = list;
  while(iterator != NULL) {
    result++;
    iterator = iterator->nextItem;
  }
  return result;
}

void freeLinkedList(LinkedList list, LinkedListFreeItemFunc freeItem) {
  LinkedListIterator iterator = list;
  while(iterator->nextItem != NULL) {
    freeItem(iterator->item);
    LinkedList current = iterator;
    iterator = iterator->nextItem;
    free(current);
  }
}
