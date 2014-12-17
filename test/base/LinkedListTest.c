#include <stdlib.h>
#include "unit/TestRunner.h"

static char *const TEST_ITEM_STRING = "test string";
static char *const OTHER_TEST_ITEM_STRING = "other test string";

static boolByte _gNumForeachCallbacksMade;
static boolByte _gForeachCallbackOk;

static void _linkedListTestSetup(void)
{
    _gNumForeachCallbacksMade = 0;
    _gForeachCallbackOk = false;
}

static int _testNewLinkedList(void)
{
    LinkedList l = newLinkedList();
    assertNotNull(l);
    assertIsNull(l->nextItem);
    assertIntEquals(0, linkedListLength(l));
    assert(l->item == NULL);
    freeLinkedList(l);
    return 0;
}

static int _testAppendItemToList(void)
{
    LinkedList l = newLinkedList();
    CharString c = newCharString();
    charStringCopyCString(c, TEST_ITEM_STRING);
    linkedListAppend(l, c);
    assertNotNull(l->item);
    assertCharStringEquals(TEST_ITEM_STRING, ((CharString)l->item));
    assertIsNull(l->nextItem);
    freeLinkedListAndItems(l, (LinkedListFreeItemFunc)freeCharString);
    return 0;
}

static int _testAppendMultipleItemsToList(void)
{
    LinkedListIterator i;
    LinkedList l = newLinkedList();
    CharString c = newCharString();
    CharString c2 = newCharString();

    charStringCopyCString(c, TEST_ITEM_STRING);
    charStringCopyCString(c2, OTHER_TEST_ITEM_STRING);
    linkedListAppend(l, c);
    linkedListAppend(l, c2);
    assertNotNull(l->item);
    assertCharStringEquals(TEST_ITEM_STRING, ((CharString)l->item));
    assertNotNull(l->nextItem);
    i = (LinkedListIterator)(l->nextItem);
    assertNotNull(i->item);
    assertCharStringEquals(OTHER_TEST_ITEM_STRING, ((CharString)i->item));
    assertIsNull(i->nextItem);
    assertIntEquals(2, linkedListLength(l));

    freeLinkedListAndItems(l, (LinkedListFreeItemFunc)freeCharString);
    return 0;
}

static int _testAppendNullItemToList(void)
{
    LinkedList l = newLinkedList();
    linkedListAppend(l, NULL);
    assertIsNull(l->item);
    assertIsNull(l->nextItem);
    assertIntEquals(0, linkedListLength(l));
    freeLinkedList(l);
    return 0;
}

static int _testAppendItemToNullList(void)
{
    CharString c = newCharString();
    // The test here is not to crash
    linkedListAppend(NULL, c);
    freeCharString(c);
    return 0;
}

static int _testNumItemsInList(void)
{
    CharString c;
    int i;
    LinkedList l = newLinkedList();

    for (i = 0; i < 100; i++) {
        c = newCharString();
        charStringCopyCString(c, TEST_ITEM_STRING);
        linkedListAppend(l, c);
    }

    assertIntEquals(100, linkedListLength(l));
    freeLinkedListAndItems(l, (LinkedListFreeItemFunc)freeCharString);
    return 0;
}

static int _testNumItemsInNullList(void)
{
    assertIntEquals(0, linkedListLength(NULL));
    return 0;
}

static int _testLinkedListToArray(void)
{
    LinkedList l = newLinkedList();
    CharString *arr;
    CharString c;

    linkedListAppend(l, newCharStringWithCString("one"));
    linkedListAppend(l, newCharStringWithCString("two"));

    arr = (CharString *)linkedListToArray(l);
    assertNotNull(arr);
    c = (CharString)arr[0];
    assertCharStringEquals("one", c);
    c = (CharString)arr[1];
    assertCharStringEquals("two", c);
    assertIsNull(arr[2]);

    free(arr);
    freeLinkedListAndItems(l, (LinkedListFreeItemFunc)freeCharString);
    return 0;
}

static int _testLinkedListToArrayWithNull(void)
{
    CharString **arr;
    arr = (CharString **)linkedListToArray(NULL);
    assertIsNull(arr);
    return 0;
}

static int _testLinkedListWithEmptyList(void)
{
    CharString **arr;
    LinkedList l = newLinkedList();
    arr = (CharString **)linkedListToArray(l);
    assertIsNull(arr);
    freeLinkedList(l);
    return 0;
}


static void _linkedListEmptyCallback(void *item, void *userData)
{
    _gNumForeachCallbacksMade++;
}

static int _testForeachOverNullList(void)
{
    linkedListForeach(NULL, _linkedListEmptyCallback, NULL);
    assertIntEquals(0, _gNumForeachCallbacksMade);
    return 0;
}

static int _testForeachOverEmptyList(void)
{
    LinkedList l = newLinkedList();
    linkedListForeach(l, _linkedListEmptyCallback, NULL);
    assertIntEquals(0, _gNumForeachCallbacksMade);
    freeLinkedList(l);
    return 0;
}

static void _linkedListTestStringCallback(void *item, void *userData)
{
    CharString charString = (CharString)item;
    _gForeachCallbackOk = charStringIsEqualToCString(charString, TEST_ITEM_STRING, false);
    _gNumForeachCallbacksMade++;
}

static int _testForeachOverList(void)
{
    LinkedList l = newLinkedList();
    CharString c = newCharString();

    charStringCopyCString(c, TEST_ITEM_STRING);
    linkedListAppend(l, c);
    linkedListForeach(l, _linkedListTestStringCallback, NULL);
    assertIntEquals(1, _gNumForeachCallbacksMade);
    assert(_gForeachCallbackOk);

    freeLinkedListAndItems(l, (LinkedListFreeItemFunc)freeCharString);
    return 0;
}

static void _linkedListUserDataCallback(void *item, void *userData)
{
    CharString charString = (CharString)userData;
    _gForeachCallbackOk = charStringIsEqualToCString(charString, TEST_ITEM_STRING, false);
    _gNumForeachCallbacksMade++;
}

static int _testForeachOverUserData(void)
{
    LinkedList l = newLinkedList();
    CharString c = newCharString();

    charStringCopyCString(c, TEST_ITEM_STRING);
    linkedListAppend(l, c);
    linkedListForeach(l, _linkedListUserDataCallback, c);
    assertIntEquals(1, _gNumForeachCallbacksMade);
    assert(_gForeachCallbackOk);

    freeLinkedListAndItems(l, (LinkedListFreeItemFunc)freeCharString);
    return 0;
}

static int _testFreeNullLinkedList(void)
{
    freeLinkedList(NULL);
    return 0;
}

TestSuite addLinkedListTests(void);
TestSuite addLinkedListTests(void)
{
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
