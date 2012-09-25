//
// LinkedList.c - MrsWatson
// Created by Nik Reiman on 1/3/12.
// Copyright (c) 2012 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"
#include "Types.h"

LinkedList newLinkedList(void) {
  LinkedList list = malloc(sizeof(LinkedListMembers));

  list->item = NULL;
  list->nextItem = NULL;
  list->_numItems = 0;

  return list;
}

void appendItemToList(LinkedList list, void* item) {
  LinkedListIterator iterator = list;
  LinkedListIterator headNode;
  LinkedList nextItem;

  if(list == NULL || item == NULL) {
    return;
  }

  // First item in the list
  if(iterator->item == NULL) {
    iterator->item = item;
    iterator->_numItems = 1;
    return;
  }

  headNode = list;
  while(true) {
    if(iterator->nextItem == NULL) {
      nextItem = newLinkedList();
      nextItem->item = item;
      iterator->nextItem = nextItem;
      headNode->_numItems++;
      break;
    }
    else {
      iterator = (LinkedListIterator)(iterator->nextItem);
    }
  }
}

int numItemsInList(LinkedList list) {
  return list != NULL ? list->_numItems : 0;
}

void foreachItemInList(LinkedList list, LinkedListForeachFunc foreachFunc, void* userData) {
  LinkedListIterator iterator = list;

  while(iterator != NULL) {
    if(iterator->item != NULL) {
      foreachFunc(iterator->item, userData);
    }
    iterator = iterator->nextItem;
  }
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
  LinkedList current;

  if(iterator->item == NULL){
    free(iterator);
    return;
  }

  while(true) {
    if(iterator->nextItem == NULL) {
      freeItem(iterator->item);
      free(iterator);
      break;
    }
    else {
      freeItem(iterator->item);
      current = iterator;
      iterator = (LinkedListIterator)(iterator->nextItem);
      free(current);
    }
  }
}
