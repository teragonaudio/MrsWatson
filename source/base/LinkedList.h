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

/**
 * Create a new linked list
 * @return Linked list with no items
 */
LinkedList newLinkedList(void);

/**
 * Add an item to the end of a list
 * @param self
 * @param item Item to append. Items should (but do not necessarily have to be)
 * of the same type. However, if items in the list are not of the same type,
 * using functions such as foreachItemInList() will be much more difficult.
 */
void linkedListAppend(LinkedList self, void* item);

/**
 * Get the number of items in a list. Use this function instead of accessing
 * the fields of the list, as the field names or use may change in the future.
 * @param self
 * @return Number of items in the list
 */
int linkedListLength(LinkedList self);

/**
 * Flatten a LinkedList to an array. The resulting array will be size N + 1,
 * with a NULL object at the end of the array.
 * @param self
 * @return Array of void* objects with terminating NULL member
 */
void** linkedListToArray(LinkedList self);

/**
 * Iterate over each item in a linked list, calling the given function on each item.
 * @param self
 * @param foreachFunc Function to call
 * @param userData User data to pass to the function
 */
void linkedListForeach(LinkedList self, LinkedListForeachFunc foreachFunc, void* userData);

/**
 * Free each item in a linked list. The contents of the items themselves are *not*
 * freed. To do that, call freeLinkedListAndItems() instead.
 * @param self
 */
void freeLinkedList(LinkedList self);

/**
 * Free a linked list and each of its items.
 * @param self
 * @param freeItem Free function to be called for each item
 */
void freeLinkedListAndItems(LinkedList self, LinkedListFreeItemFunc freeItem);

#endif
