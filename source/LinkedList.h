//
//  LinkedList.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#ifndef MrsWatson_LinkedList_h
#define MrsWatson_LinkedList_h

typedef struct {
  void* item;
  void* nextItem;
} LinkedListMembers;

typedef LinkedListMembers *LinkedList;
typedef LinkedListMembers *LinkedListIterator;

typedef void (*LinkedListFreeItemFunc)(void*);

LinkedList newLinkedList(void);
void appendItemToList(LinkedList list, void* item);
int numItemsInList(LinkedList list);
void freeLinkedList(LinkedList list, LinkedListFreeItemFunc freeItem);

#endif
