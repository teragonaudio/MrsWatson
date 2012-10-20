#include "TestRunner.h"
#include "FileUtilities.h"

static const char* TEST_FILENAME = "fileExistsTest.txt";
static const char* ABSOLUTE_TEST_FILENAME = "/tmp/fileExistsTest.txt";

static int _testFileExists(void) {
  FILE *fp = fopen(TEST_FILENAME, "w");
  assert(fp != NULL);
  fclose(fp);
  assert(fileExists(TEST_FILENAME));
  unlink(TEST_FILENAME);
  return 0;
}

static int _testNullFileExists(void) {
  assertFalse(fileExists(NULL));
  return 0;
}

static int _testInvalidFileExists(void) {
  assertFalse(fileExists("invalid"));
  return 0;
}

static int _testCopyFileToDirectory(void) {
  return 0;
}

static int _testCopyInvalidFileToDirectory(void) {
  return 0;
}

static int _testCopyFileToInvalidDirectory(void) {
  return 0;
}

static int _testListDirectory(void) {
  return 0;
}

static int _testListEmptyDirectory(void) {
  return 0;
}

static int _testListInvalidDirectory(void) {
  return 0;
}

static int _testRemoveDirectory(void) {
  return 0;
}

static int _testRemoveInvalidDirectory(void) {
  return 0;
}

static int _testBuildAbsolutePath(void) {
  return 0;
}

static int _testBuildAbsolutePathWithFileExtension(void) {
  return 0;
}

static int _testIsAbsolutePath(void) {
  assert(isAbsolutePath(newCharStringWithCString(ABSOLUTE_TEST_FILENAME)));
  return 0;
}

static int _testIsInvalidFileAbsolutePath(void) {
  assertFalse(isAbsolutePath(newCharStringWithCString("invalid")));
  return 0;
}

static int _testGetFileBasename(void) {
  assertCharStringEquals(newCharStringWithCString(getFileBasename(ABSOLUTE_TEST_FILENAME)), TEST_FILENAME);
  return 0;
}

static int _testGetNullFileBasename(void) {
  assertIsNull(getFileBasename(NULL));
  return 0;
}

static int _testGetFileExtension(void) {
  assertCharStringEquals(newCharStringWithCString(getFileExtension(TEST_FILENAME)), "txt");
  return 0;
}

static int _testGetNullFileExtension(void) {
  assertIsNull(getFileExtension(NULL));
  return 0;
}

static int _testGetInvalidFileExtension(void) {
  assertIsNull(getFileExtension("invalid"));
  return 0;
}

TestSuite addFileUtilitiesTests(void);
TestSuite addFileUtilitiesTests(void) {
  TestSuite testSuite = newTestSuite("FileUtilities", NULL, NULL);
  addTest(testSuite, "FileExists", _testFileExists);
  addTest(testSuite, "NullFileExists", _testNullFileExists);
  addTest(testSuite, "InvalidFileExists", _testInvalidFileExists);
  addTest(testSuite, "CopyFileToDirectory", NULL); // _testCopyFileToDirectory);
  addTest(testSuite, "CopyInvalidFileToDirectory", NULL); // _testCopyInvalidFileToDirectory);
  addTest(testSuite, "CopyFileToInvalidDirectory", NULL); // _testCopyFileToInvalidDirectory);

  addTest(testSuite, "ListDirectory", NULL); // _testListDirectory);
  addTest(testSuite, "ListEmptyDirectory", NULL); // _testListEmptyDirectory);
  addTest(testSuite, "ListInvalidDirectory", NULL); // _testListInvalidDirectory);
  addTest(testSuite, "RemoveDirectory", NULL); // _testRemoveDirectory);
  addTest(testSuite, "RemoveInvalidDirectory", NULL); // _testRemoveInvalidDirectory);

  addTest(testSuite, "BuildAbsolutePath", NULL); // _testBuildAbsolutePath);
  addTest(testSuite, "BuildAbsolutePathWithFileExtension", NULL); // _testBuildAbsolutePathWithFileExtension);
  addTest(testSuite, "IsAbsolutePath", _testIsAbsolutePath);
  addTest(testSuite, "IsInvalidFileAbsolutePath", _testIsInvalidFileAbsolutePath);

  addTest(testSuite, "GetFileBasename", _testGetFileBasename);
  addTest(testSuite, "GetNullFileBasename", _testGetNullFileBasename);
  addTest(testSuite, "GetFileExtension", _testGetFileExtension);
  addTest(testSuite, "GetNullFileExtension", _testGetNullFileExtension);
  addTest(testSuite, "GetInvalidFileExtension", _testGetInvalidFileExtension);
  return testSuite;
}
