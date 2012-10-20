#include "TestRunner.h"
#include "LinkedList.h"

static char *const TEST_ITEM_STRING = "test string";
static char *const OTHER_TEST_ITEM_STRING = "other test string";

static boolByte _gNumForeachCallbacksMade;
static boolByte _gForeachCallbackOk;

static void _linkedListTestSetup(void) {
  _gNumForeachCallbacksMade = 0;
  _gForeachCallbackOk = false;
}

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

static void _linkedListEmptyCallback(void* item, void* userData) {
  _gNumForeachCallbacksMade++;
}

static int _testForeachOverNullList(void) {
  foreachItemInList(NULL, _linkedListEmptyCallback, NULL);
  _assertIntEquals(_gNumForeachCallbacksMade, 0);
  return 0;
}

static int _testForeachOverEmptyList(void) {
  LinkedList list = newLinkedList();
  foreachItemInList(list, _linkedListEmptyCallback, NULL);
  _assertIntEquals(_gNumForeachCallbacksMade, 0);
  return 0;
}

static void _linkedListTestStringCallback(void* item, void* userData) {
  CharString charString = (CharString)item;
  _gForeachCallbackOk = isCharStringEqualToCString(charString, TEST_ITEM_STRING, false);
  _gNumForeachCallbacksMade++;
}

static int _testForeachOverList(void) {
  LinkedList list = newLinkedList();
  CharString charString = newCharString();
  copyToCharString(charString, TEST_ITEM_STRING);
  appendItemToList(list, charString);
  foreachItemInList(list, _linkedListTestStringCallback, NULL);
  _assertIntEquals(_gNumForeachCallbacksMade, 1);
  _assert(_gForeachCallbackOk);
  return 0;
}

static void _linkedListUserDataCallback(void* item, void* userData) {
  CharString charString = (CharString)userData;
  _gForeachCallbackOk = isCharStringEqualToCString(charString, TEST_ITEM_STRING, false);
  _gNumForeachCallbacksMade++;
}

static int _testForeachOverUserData(void) {
  LinkedList list = newLinkedList();
  CharString charString = newCharString();
  copyToCharString(charString, TEST_ITEM_STRING);
  appendItemToList(list, charString);
  foreachItemInList(list, _linkedListUserDataCallback, charString);
  _assertIntEquals(_gNumForeachCallbacksMade, 1);
  _assert(_gForeachCallbackOk);
  return 0;
}

TestSuite addLinkedListTests(void);
TestSuite addLinkedListTests(void) {
  TestSuite testSuite = newTestSuite("LinkedList", NULL, NULL);
  testSuite->setup = _linkedListTestSetup;
  addTest(testSuite, "New object", _testNewLinkedList);
  addTest(testSuite, "Append item", _testAppendItemToList);
  addTest(testSuite, "Append multiple items", _testAppendMultipleItemsToList);
  addTest(testSuite, "Append null item", _testAppendNullItemToList);
  addTest(testSuite, "Append item to null list", _testAppendItemToNullList);
  addTest(testSuite, "Num items in list", _testNumItemsInList);
  addTest(testSuite, "Num items in null list", _testNumItemsInNullList);
  addTest(testSuite, "Foreach over null list", _testForeachOverNullList);
  addTest(testSuite, "Foreach over empty list", _testForeachOverEmptyList);
  addTest(testSuite, "Foreach over list", _testForeachOverList);
  addTest(testSuite, "Foreach with userData", _testForeachOverUserData)
  return testSuite;
}
