#include "unit/TestRunner.h"
#include "base/LinkedList.h"

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
  assertNotNull(l);
  assertIsNull(l->nextItem);
  assertIntEquals(numItemsInList(l), 0);
  assert(l->item == NULL);
  return 0;
}

static int _testAppendItemToList(void) {
  LinkedList l = newLinkedList();
  CharString c = newCharString();
  charStringCopyCString(c, TEST_ITEM_STRING);
  appendItemToList(l, c);
  assertNotNull(l->item);
  assertCharStringEquals(((CharString)l->item), TEST_ITEM_STRING);
  assertIsNull(l->nextItem);
  return 0;
}

static int _testAppendMultipleItemsToList(void) {
  LinkedListIterator i;
  LinkedList l = newLinkedList();
  CharString c = newCharString();
  CharString c2 = newCharString();
  charStringCopyCString(c, TEST_ITEM_STRING);
  charStringCopyCString(c2, OTHER_TEST_ITEM_STRING);
  appendItemToList(l, c);
  appendItemToList(l, c2);
  assertNotNull(l->item);
  assertCharStringEquals(((CharString)l->item), TEST_ITEM_STRING);
  assertNotNull(l->nextItem);
  i = l->nextItem;
  assertNotNull(i->item);
  assertCharStringEquals(((CharString)i->item), OTHER_TEST_ITEM_STRING);
  assertIsNull(i->nextItem);
  assertIntEquals(numItemsInList(l), 2);
  return 0;
}

static int _testAppendNullItemToList(void) {
  LinkedList l = newLinkedList();
  appendItemToList(l, NULL);
  assertIsNull(l->item);
  assertIsNull(l->nextItem);
  assertIntEquals(numItemsInList(l), 0);
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
    charStringCopyCString(c, TEST_ITEM_STRING);
    appendItemToList(l, c);
  }
  assertIntEquals(numItemsInList(l), 100);
  return 0;
}

static int _testNumItemsInNullList(void) {
  assertIntEquals(numItemsInList(NULL), 0);
  return 0;
}

static int _testLinkedListToArray(void) {
  LinkedList l = newLinkedList();
  CharString* arr;

  appendItemToList(l, newCharStringWithCString("one"));
  appendItemToList(l, newCharStringWithCString("two"));
  arr = (CharString*)linkedListToArray(l);
  assertNotNull(arr);
  assertCharStringEquals((CharString)arr[0], "one");
  assertCharStringEquals((CharString)arr[1], "two");
  assertIsNull(arr[2]);

  return 0;
}

static int _testLinkedListToArrayWithNull(void) {
  CharString** arr;
  arr = (CharString**)linkedListToArray(NULL);
  assertIsNull(arr);
  return 0;
}

static int _testLinkedListWithEmptyList(void) {
  CharString** arr;
  LinkedList l = newLinkedList();
  arr = (CharString**)linkedListToArray(l);
  assertIsNull(arr);
  return 0;
}

static void _linkedListEmptyCallback(void* item, void* userData) {
  _gNumForeachCallbacksMade++;
}

static int _testForeachOverNullList(void) {
  foreachItemInList(NULL, _linkedListEmptyCallback, NULL);
  assertIntEquals(_gNumForeachCallbacksMade, 0);
  return 0;
}

static int _testForeachOverEmptyList(void) {
  LinkedList list = newLinkedList();
  foreachItemInList(list, _linkedListEmptyCallback, NULL);
  assertIntEquals(_gNumForeachCallbacksMade, 0);
  return 0;
}

static void _linkedListTestStringCallback(void* item, void* userData) {
  CharString charString = (CharString)item;
  _gForeachCallbackOk = charStringIsEqualToCString(charString, TEST_ITEM_STRING, false);
  _gNumForeachCallbacksMade++;
}

static int _testForeachOverList(void) {
  LinkedList list = newLinkedList();
  CharString charString = newCharString();
  charStringCopyCString(charString, TEST_ITEM_STRING);
  appendItemToList(list, charString);
  foreachItemInList(list, _linkedListTestStringCallback, NULL);
  assertIntEquals(_gNumForeachCallbacksMade, 1);
  assert(_gForeachCallbackOk);
  return 0;
}

static void _linkedListUserDataCallback(void* item, void* userData) {
  CharString charString = (CharString)userData;
  _gForeachCallbackOk = charStringIsEqualToCString(charString, TEST_ITEM_STRING, false);
  _gNumForeachCallbacksMade++;
}

static int _testForeachOverUserData(void) {
  LinkedList list = newLinkedList();
  CharString charString = newCharString();
  charStringCopyCString(charString, TEST_ITEM_STRING);
  appendItemToList(list, charString);
  foreachItemInList(list, _linkedListUserDataCallback, charString);
  assertIntEquals(_gNumForeachCallbacksMade, 1);
  assert(_gForeachCallbackOk);
  return 0;
}

static int _testFreeNullLinkedList(void) {
  freeLinkedList(NULL);
  return 0;
}

TestSuite addLinkedListTests(void);
TestSuite addLinkedListTests(void) {
  TestSuite testSuite = newTestSuite("LinkedList", _linkedListTestSetup, NULL);
  addTest(testSuite, "NewObject", _testNewLinkedList);

  addTest(testSuite, "AppendItem", _testAppendItemToList);
  addTest(testSuite, "AppendMultipleItems", _testAppendMultipleItemsToList);
  addTest(testSuite, "AppendNullItem", _testAppendNullItemToList);
  addTest(testSuite, "AppendItemToNullList", _testAppendItemToNullList);

  addTest(testSuite, "NumItemsInList", _testNumItemsInList);
  addTest(testSuite, "NumItemsInNullList", _testNumItemsInNullList);

  addTest(testSuite, "LinkedListToArray", _testLinkedListToArray);
  addTest(testSuite, "LinkedListToArrayWithNull", _testLinkedListToArrayWithNull);
  addTest(testSuite, "LinkedListWithEmptyList", _testLinkedListWithEmptyList);

  addTest(testSuite, "ForeachOverNullList", _testForeachOverNullList);
  addTest(testSuite, "ForeachOverEmptyList", _testForeachOverEmptyList);
  addTest(testSuite, "ForeachOverList", _testForeachOverList);
  addTest(testSuite, "ForeachWithUserData", _testForeachOverUserData);

  addTest(testSuite, "FreeNullLinkedList", _testFreeNullLinkedList);

  return testSuite;
}
