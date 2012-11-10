//
// LinkedList.h - MrsWatson
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

#ifndef MrsWatson_LinkedList_h
#define MrsWatson_LinkedList_h

typedef struct {
  void* item;
  void* nextItem;

  // This field should be considered private, and is only valid for the head node
  int _numItems;
} LinkedListMembers;

typedef LinkedListMembers *LinkedList;
typedef LinkedListMembers *LinkedListIterator;

typedef void (*LinkedListForeachFunc)(void* item, void* userData);
typedef void (*LinkedListFreeItemFunc)(void* item);

LinkedList newLinkedList(void);
LinkedList newLinkedListWithItems(void* firstItem, ...);

void appendItemToList(LinkedList list, void* item);
int numItemsInList(LinkedList list);
// TODO: Test cases
void foreachItemInList(LinkedList list, LinkedListForeachFunc foreachFunc, void* userData);

void freeLinkedList(LinkedList list);
void freeLinkedListAndItems(LinkedList list, LinkedListFreeItemFunc freeItem);

#endif
