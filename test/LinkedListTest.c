#include "TestRunner.h"
#include "LinkedList.h"

static char *const TEST_ITEM_STRING = "test string";
static char *const OTHER_TEST_ITEM_STRING = "other test string";

static int _testNewLinkedList(void) {
  LinkedList l = newLinkedList();
  _assertNotNull(l);
  _assertIsNull(l->nextItem);
  _assertIntEquals(numItemsInList(l), 0);
  _assert(l->item == NULL);
  return 0;
}

static int _testAppendItemToList(void) {
  LinkedList l = newLinkedList();
  CharString c = newCharString();
  copyToCharString(c, TEST_ITEM_STRING);
  appendItemToList(l, c);
  _assertNotNull(l->item);
  _assertCharStringEquals(((CharString)l->item), TEST_ITEM_STRING);
  _assertIsNull(l->nextItem);
  return 0;
}

static int _testAppendMultipleItemsToList(void) {
  LinkedListIterator i;
  LinkedList l = newLinkedList();
  CharString c = newCharString();
  CharString c2 = newCharString();
  copyToCharString(c, TEST_ITEM_STRING);
  copyToCharString(c2, OTHER_TEST_ITEM_STRING);
  appendItemToList(l, c);
  appendItemToList(l, c2);
  _assertNotNull(l->item);
  _assertCharStringEquals(((CharString)l->item), TEST_ITEM_STRING);
  _assertNotNull(l->nextItem);
  i = l->nextItem;
  _assertNotNull(i->item);
  _assertCharStringEquals(((CharString)i->item), OTHER_TEST_ITEM_STRING);
  _assertIsNull(i->nextItem);
  _assertIntEquals(numItemsInList(l), 2);
  return 0;
}

static int _testAppendNullItemToList(void) {
  LinkedList l = newLinkedList();
  appendItemToList(l, NULL);
  _assertIsNull(l->item);
  _assertIsNull(l->nextItem);
  _assertIntEquals(numItemsInList(l), 0);
  return 0;
}

static int _testAppendItemToNullList(void) {
  CharString c = newCharString();
  // The test here is not to crash
  appendItemToList(NULL, c);
  return 0;
}

static int _testNumItemsInList(void) {
  CharString c;
  int i;
  LinkedList l = newLinkedList();
  for(i = 0; i < 100; i++) {
    c = newCharString();
    copyToCharString(c, TEST_ITEM_STRING);
    appendItemToList(l, c);
  }
  _assertIntEquals(numItemsInList(l), 100);
  return 0;
}

static int _testNumItemsInNullList(void) {
  _assertIntEquals(numItemsInList(NULL), 0);
  return 0;
}

void runLinkedListTests(void);
void runLinkedListTests(void) {
  _startTestSection();
  _runTest("New object", _testNewLinkedList, emptySetup, emptyTeardown);
  _runTest("Append item", _testAppendItemToList, emptySetup, emptyTeardown);
  _runTest("Append multiple items", _testAppendMultipleItemsToList, emptySetup, emptyTeardown);
  _runTest("Append null item", _testAppendNullItemToList, emptySetup, emptyTeardown);
  _runTest("Append item to null list", _testAppendItemToNullList, emptySetup, emptyTeardown);
  _runTest("Num items in list", _testNumItemsInList, emptySetup, emptyTeardown);
  _runTest("Num items in null list", _testNumItemsInNullList, emptySetup, emptyTeardown);
}
