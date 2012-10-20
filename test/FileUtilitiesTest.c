#include "TestRunner.h"
#include "FileUtilities.h"

static int _testFileExists(void) {
  return 0;
}

static int _testNullFileExists(void) {
  return 0;
}

static int _testInvalidFileExists(void) {
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
  return 0;
}

static int _testIsInvalidFileAbsolutePath(void) {
  return 0;
}

static int _testGetFileBasename(void) {
  return 0;
}

static int _testGetNullFileBasename(void) {
  return 0;
}

static int _testGetFileExtension(void) {
  return 0;
}

static int _testGetNullFileExtension(void) {
  return 0;
}

static int _testGetInvalidFileExtension(void) {
  return 0;
}

TestSuite addFileUtilitiesTests(void);
TestSuite addFileUtilitiesTests(void) {
  TestSuite testSuite = newTestSuite("FileUtilities", NULL, NULL);
  addTest(testSuite, "FileExists", NULL); // _testFileExists);
  addTest(testSuite, "NullFileExists", NULL); // _testNullFileExists);
  addTest(testSuite, "InvalidFileExists", NULL); // _testInvalidFileExists);
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
  addTest(testSuite, "IsAbsolutePath", NULL); // _testIsAbsolutePath);
  addTest(testSuite, "IsInvalidFileAbsolutePath", NULL); // _testIsInvalidFileAbsolutePath);

  addTest(testSuite, "GetFileBasename", NULL); // _testGetFileBasename);
  addTest(testSuite, "GetNullFileBasename", NULL); // _testGetNullFileBasename);
  addTest(testSuite, "GetFileExtension", NULL); // _testGetFileExtension);
  addTest(testSuite, "GetNullFileExtension", NULL); // _testGetNullFileExtension);
  addTest(testSuite, "GetInvalidFileExtension", NULL); // _testGetInvalidFileExtension);
  return testSuite;
}
