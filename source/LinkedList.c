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
#include "Types.h"

LinkedList newLinkedList(void) {
  LinkedList list = malloc(sizeof(LinkedListMembers));

  list->item = NULL;
  list->nextItem = NULL;

  return list;
}

void appendItemToList(LinkedList list, void* item) {
  LinkedListIterator iterator = list;

  // First item in the list
  if(iterator->item == NULL) {
    iterator->item = item;
    return;
  }

  while(true) {
    if(iterator->nextItem == NULL) {
      LinkedList nextItem = newLinkedList();
      nextItem->item = item;
      iterator->nextItem = nextItem;
      break;
    }
    else {
      iterator = iterator->nextItem;
    }
  }
}

// TODO: This is a bit inefficent
int numItemsInList(LinkedList list) {
  int result = 0;
  LinkedListIterator iterator = list;
  while(iterator->item != NULL && iterator->nextItem != NULL) {
    result++;
    iterator = iterator->nextItem;
  }
  return result;
}

void freeLinkedList(LinkedList list) {
  LinkedListIterator iterator = list;
  while(true) {
    if(iterator->nextItem == NULL) {
      free(iterator);
      break;
    }
    else {
      LinkedList current = iterator;
      iterator = iterator->nextItem;
      free(current);
    }
  }
}

void freeLinkedListAndItems(LinkedList list, LinkedListFreeItemFunc freeItem) {
  LinkedListIterator iterator = list;
  while(true) {
    if(iterator->nextItem == NULL) {
      freeItem(iterator->item);
      free(iterator);
      break;
    }
    else {
      freeItem(iterator->item);
      LinkedList current = iterator;
      iterator = iterator->nextItem;
      free(current);
    }
  }
}
