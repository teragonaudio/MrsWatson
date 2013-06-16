#include "unit/TestRunner.h"
#include "base/FileUtilities.h"

#include <stdio.h>
#include <stdlib.h>
#if UNIX
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

static const char* TEST_FILENAME = "fileUtilitiesTest.txt";
#if UNIX
static const char* ABSOLUTE_TEST_FILENAME = "/tmp/fileUtilitiesTest.txt";
#elif WINDOWS
static const char* ABSOLUTE_TEST_FILENAME = "C:\\Temp\\fileUtilitiesTest.txt";
#else
static const char* ABSOLUTE_TEST_FILENAME = "fileUtilitiesTest.txt";
#endif

static int _testFileExists(void) {
  FILE *fp = fopen(TEST_FILENAME, "w");
  assert(fp != NULL);
  fclose(fp);
  assert(_fileExists(TEST_FILENAME));
  unlink(TEST_FILENAME);
  return 0;
}

static int _testNullFileExists(void) {
  assertFalse(_fileExists(NULL));
  return 0;
}

static int _testInvalidFileExists(void) {
  assertFalse(_fileExists("invalid"));
  return 0;
}

static CharString _fileUtilitiesMakeTempDir(void) {
  CharString tempDirName = newCharString();
#if UNIX
  snprintf(tempDirName->data, tempDirName->capacity, "/tmp/mrswatsontest-XXXXXX");
  mktemp(tempDirName->data);
#elif WINDOWS
  CharString systemTempDir = newCharString();
  CharString randomDirName = newCharString();
  snprintf(randomDirName->data, randomDirName->capacity, "mrswatsontest-%d", rand());
  GetTempPathA(systemTempDir->capacity, systemTempDir->data);
  buildAbsolutePath(systemTempDir, randomDirName, NULL, tempDirName);
  freeCharString(systemTempDir);
  freeCharString(randomDirName);
#endif
  if(!makeDirectory(tempDirName)) {
    fprintf(stderr, "WARNING: Could not make temporary directory\n");
    return NULL;
  }
  return tempDirName;
}

static int _testCopyFileToDirectory(void) {
  CharString tempDir = _fileUtilitiesMakeTempDir();
  CharString tempFile = newCharString();
  CharString testFilename = newCharStringWithCString(TEST_FILENAME);

  FILE *fp = fopen(TEST_FILENAME, "w");
  assertNotNull(fp);
  fclose(fp);
  convertRelativePathToAbsolute(testFilename, tempFile);
  assert(copyFileToDirectory(tempFile, tempDir));
  assert(removeDirectory(tempDir));

  unlink(TEST_FILENAME);
  freeCharString(tempDir);
  freeCharString(tempFile);
  freeCharString(testFilename);
  return 0;
}

static int _testCopyInvalidFileToDirectory(void) {
  CharString tempDir = _fileUtilitiesMakeTempDir();
  CharString testFilename = newCharStringWithCString(TEST_FILENAME);
  CharString invalid = newCharStringWithCString("invalid");

  convertRelativePathToAbsolute(testFilename, invalid);
  assertFalse(copyFileToDirectory(invalid, tempDir));
  removeDirectory(tempDir);

  freeCharString(testFilename);
  freeCharString(tempDir);
  freeCharString(invalid);
  return 0;
}

static int _testCopyFileToInvalidDirectory(void) {
  CharString tempFile = newCharString();
  CharString testFilename = newCharStringWithCString(TEST_FILENAME);
  CharString invalid = newCharStringWithCString("invalid");
  FILE *fp = fopen(TEST_FILENAME, "w");

  assertNotNull(fp);
  fclose(fp);
  convertRelativePathToAbsolute(testFilename, tempFile);
  assertFalse(copyFileToDirectory(tempFile, invalid));

  unlink(TEST_FILENAME);
  freeCharString(tempFile);
  freeCharString(testFilename);
  freeCharString(invalid);
  return 0;
}

static int _testListDirectory(void) {
  CharString tempDir = _fileUtilitiesMakeTempDir();
  CharString tempFile = newCharString();
  CharString filename;
  CharString testFilename = newCharStringWithCString(TEST_FILENAME);
  LinkedList l;
  FILE *f;

  buildAbsolutePath(tempDir, testFilename, NULL, tempFile);
  f = fopen(tempFile->data, "w");
  assertNotNull(f);
  fclose(f);
  l = listDirectory(tempDir);
  assertIntEquals(linkedListLength(l), 1);
  filename = (CharString)l->item;
  assertCharStringEquals(filename, TEST_FILENAME);

  removeDirectory(tempDir);
  freeCharString(tempDir);
  freeCharString(tempFile);
  freeCharString(testFilename);
  freeLinkedListAndItems(l, (LinkedListFreeItemFunc)freeCharString);
  return 0;
}

static int _testListEmptyDirectory(void) {
  CharString tempDir = _fileUtilitiesMakeTempDir();
  LinkedList l = listDirectory(tempDir);

  assertIntEquals(linkedListLength(l), 0);
  removeDirectory(tempDir);

  freeCharString(tempDir);
  freeLinkedListAndItems(l, (LinkedListFreeItemFunc)freeCharString);
  return 0;
}

static int _testListInvalidDirectory(void) {
  CharString c = newCharStringWithCString("invalid");
  LinkedList l = listDirectory(c);
  assertIntEquals(linkedListLength(l), 0);
  freeLinkedList(l);
  freeCharString(c);
  return 0;
}

static int _testRemoveDirectory(void) {
  CharString tempDir = _fileUtilitiesMakeTempDir();
  assert(_fileExists(tempDir->data));
  assert(removeDirectory(tempDir));
  assertFalse(_fileExists(tempDir->data));
  freeCharString(tempDir);
  return 0;
}

static int _testRemoveInvalidDirectory(void) {
  CharString c = newCharStringWithCString("invalid");
  assertFalse(removeDirectory(c));
  freeCharString(c);
  return 0;
}

static int _testBuildAbsolutePathEmptyPath(void) {
  CharString d = newCharString();
  CharString f = newCharStringWithCString(TEST_FILENAME);
  CharString out = newCharString();

  buildAbsolutePath(d, f, NULL, out);
  assert(charStringIsEmpty(out));

  freeCharString(d);
  freeCharString(f);
  freeCharString(out);
  return 0;
}

static int _testBuildAbsolutePathEmptyFile(void) {
  CharString d = getCurrentDirectory();
  CharString f = newCharString();
  CharString out = newCharString();

  buildAbsolutePath(d, f, NULL, out);
  assert(charStringIsEmpty(out));

  freeCharString(d);
  freeCharString(f);
  freeCharString(out);
  return 0;
}

static int _testBuildAbsolutePathNullPath(void) {
  CharString f = newCharString();
  CharString out = newCharString();

  buildAbsolutePath(NULL, f, NULL, out);
  assert(charStringIsEmpty(out));

  freeCharString(f);
  freeCharString(out);
  return 0;
}

static int _testBuildAbsolutePathNullFile(void) {
  CharString d = getCurrentDirectory();
  CharString out = newCharString();

  buildAbsolutePath(d, NULL, NULL, out);
  assert(charStringIsEmpty(out));

  freeCharString(d);
  freeCharString(out);
  return 0;
}

static int _testBuildAbsolutePathWithFileExtensionTwice(void) {
  CharString d = newCharStringWithCString(ROOT_DIRECTORY);
  CharString out = newCharString();
  CharString f = newCharStringWithCString(TEST_FILENAME);
  CharString expected = newCharString();

  snprintf(expected->data, expected->capacity, "%s%c%s", ROOT_DIRECTORY, PATH_DELIMITER, TEST_FILENAME);
  buildAbsolutePath(d, f, "txt", out);
  assertCharStringEquals(out, expected->data);

  freeCharString(d);
  freeCharString(out);
  freeCharString(f);
  freeCharString(expected);
  return 0;
}

static int _testIsAbsolutePath(void) {
  CharString c = newCharStringWithCString(ABSOLUTE_TEST_FILENAME);
  assert(isAbsolutePath(c));
  freeCharString(c);
  return 0;
}

static int _testIsAbsolutePathUNCWindows(void) {
#if WINDOWS
  assert(isAbsolutePath(newCharStringWithCString("\\\\SERVER\\file")));
#endif
  return 0;
}

static int _testIsInvalidFileAbsolutePath(void) {
  CharString c = newCharStringWithCString("invalid");
  assertFalse(isAbsolutePath(c));
  freeCharString(c);
  return 0;
}

static int _testGetFileBasename(void) {
  CharString c = newCharStringWithCString(getFileBasename(ABSOLUTE_TEST_FILENAME));
  assertCharStringEquals(c, TEST_FILENAME);
  freeCharString(c);
  return 0;
}

static int _testGetNullFileBasename(void) {
  assertIsNull(getFileBasename(NULL));
  return 0;
}

static int _testGetFileExtension(void) {
  CharString c = newCharStringWithCString(getFileExtension(TEST_FILENAME));
  assertCharStringEquals(c, "txt");
  freeCharString(c);
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

static int _testGetFileDirname(void) {
  CharString filename = newCharStringWithCString(ABSOLUTE_TEST_FILENAME);
  CharString expected = newCharString();
  CharString result = newCharString();

#if UNIX
  charStringCopyCString(expected, "/tmp");
#elif WINDOWS
  charStringCopyCString(expected, "C:\\Temp");
#endif
  getFileDirname(filename, result);
  assertCharStringEquals(result, expected->data);

  freeCharString(filename);
  freeCharString(expected);
  freeCharString(result);
  return 0;
}

static int _testGetNullFileDirname(void) {
  CharString result = newCharString();
  getFileDirname(NULL, result);
  freeCharString(result);
  return 0;
}

TestSuite addFileUtilitiesTests(void);
TestSuite addFileUtilitiesTests(void) {
  TestSuite testSuite = newTestSuite("FileUtilities", NULL, NULL);
  addTest(testSuite, "FileExists", _testFileExists);
  addTest(testSuite, "NullFileExists", _testNullFileExists);
  addTest(testSuite, "InvalidFileExists", _testInvalidFileExists);
  addTest(testSuite, "CopyFileToDirectory", _testCopyFileToDirectory);
  addTest(testSuite, "CopyInvalidFileToDirectory", _testCopyInvalidFileToDirectory);
  addTest(testSuite, "CopyFileToInvalidDirectory", _testCopyFileToInvalidDirectory);

  addTest(testSuite, "ListDirectory", _testListDirectory);
  addTest(testSuite, "ListEmptyDirectory", _testListEmptyDirectory);
  addTest(testSuite, "ListInvalidDirectory", _testListInvalidDirectory);
  addTest(testSuite, "RemoveDirectory", _testRemoveDirectory);
  addTest(testSuite, "RemoveInvalidDirectory", _testRemoveInvalidDirectory);

  addTest(testSuite, "BuildAbsolutePathEmptyPath", _testBuildAbsolutePathEmptyPath);
  addTest(testSuite, "BuildAbsolutePathEmptyFile", _testBuildAbsolutePathEmptyFile);
  addTest(testSuite, "BuildAbsolutePathNullPath", _testBuildAbsolutePathNullPath);
  addTest(testSuite, "BuildAbsolutePathNullFile", _testBuildAbsolutePathNullFile);
  addTest(testSuite, "BuildAbsolutePathWithFileExtensionTwice", _testBuildAbsolutePathWithFileExtensionTwice);
  addTest(testSuite, "IsAbsolutePath", _testIsAbsolutePath);
  addTest(testSuite, "IsAbsolutePathUNCWindows", _testIsAbsolutePathUNCWindows);
  addTest(testSuite, "IsInvalidFileAbsolutePath", _testIsInvalidFileAbsolutePath);

  addTest(testSuite, "GetFileBasename", _testGetFileBasename);
  addTest(testSuite, "GetNullFileBasename", _testGetNullFileBasename);
  addTest(testSuite, "GetFileExtension", _testGetFileExtension);
  addTest(testSuite, "GetNullFileExtension", _testGetNullFileExtension);
  addTest(testSuite, "GetInvalidFileExtension", _testGetInvalidFileExtension);
  addTest(testSuite, "GetFileDirname", _testGetFileDirname);
  addTest(testSuite, "GetNullFileDirname", _testGetNullFileDirname);
  return testSuite;
}
