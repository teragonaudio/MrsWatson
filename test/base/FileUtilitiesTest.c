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

static CharString _fileUtilitiesMakeTempDir(void) {
  CharString tempDirName = newCharString();
#if UNIX
  snprintf(tempDirName->data, tempDirName->length, "/tmp/mrswatsontest-XXXXXX");
  mktemp(tempDirName->data);
#elif WINDOWS
  CharString systemTempDir = newCharString();
  CharString randomDirName = newCharString();
  snprintf(randomDirName->data, randomDirName->length, "mrswatsontest-%d", rand());
  GetTempPathA(systemTempDir->length, systemTempDir->data);
  buildAbsolutePath(systemTempDir, randomDirName, NULL, tempDirName);
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
  FILE *fp = fopen(TEST_FILENAME, "w");
  assertNotNull(fp);
  fclose(fp);
  convertRelativePathToAbsolute(newCharStringWithCString(TEST_FILENAME), tempFile);
  assert(copyFileToDirectory(tempFile, tempDir));
  assert(removeDirectory(tempDir));
  unlink(TEST_FILENAME);
  return 0;
}

static int _testCopyInvalidFileToDirectory(void) {
  CharString tempDir = _fileUtilitiesMakeTempDir();
  convertRelativePathToAbsolute(newCharStringWithCString(TEST_FILENAME), newCharStringWithCString("invalid"));
  assertFalse(copyFileToDirectory(newCharStringWithCString("invalid"), tempDir));
  removeDirectory(tempDir);
  return 0;
}

static int _testCopyFileToInvalidDirectory(void) {
  CharString tempFile = newCharString();
  FILE *fp = fopen(TEST_FILENAME, "w");
  assertNotNull(fp);
  fclose(fp);
  convertRelativePathToAbsolute(newCharStringWithCString(TEST_FILENAME), tempFile);
  assertFalse(copyFileToDirectory(tempFile, newCharStringWithCString("invalid")));
  unlink(TEST_FILENAME);
  return 0;
}

static int _testListDirectory(void) {
  CharString tempDir = _fileUtilitiesMakeTempDir();
  CharString tempFile = newCharString();
  CharString filename;
  LinkedList l;
  FILE *f;

  buildAbsolutePath(tempDir, newCharStringWithCString(TEST_FILENAME), NULL, tempFile);
  f = fopen(tempFile->data, "w");
  assertNotNull(f);
  fclose(f);
  l = listDirectory(tempDir);
  assertIntEquals(linkedListLength(l), 1);
  filename = l->item;
  assertCharStringEquals(filename, TEST_FILENAME);
  removeDirectory(tempDir);
  return 0;
}

static int _testListEmptyDirectory(void) {
  CharString tempDir = _fileUtilitiesMakeTempDir();
  LinkedList l = listDirectory(tempDir);
  assertIntEquals(linkedListLength(l), 0);
  removeDirectory(tempDir);
  return 0;
}

static int _testListInvalidDirectory(void) {
  LinkedList l = listDirectory(newCharStringWithCString("invalid"));
  assertIntEquals(linkedListLength(l), 0);
  return 0;
}

static int _testRemoveDirectory(void) {
  CharString tempDir = _fileUtilitiesMakeTempDir();
  assert(fileExists(tempDir->data));
  assert(removeDirectory(tempDir));
  assertFalse(fileExists(tempDir->data));
  return 0;
}

static int _testRemoveInvalidDirectory(void) {
  assertFalse(removeDirectory(newCharStringWithCString("invalid")));
  return 0;
}

static int _testBuildAbsolutePath(void) {
  return 0;
}

static int _testBuildAbsolutePathWithFileExtension(void) {
  return 0;
}

static int _testBuildAbsolutePathEmptyPath(void) {
  return 0;
}

static int _testBuildAbsolutePathEmptyFile(void) {
  return 0;
}

static int _testBuildAbsolutePathWithFileExtensionTwice(void) {
  CharString d = newCharStringWithCString(ROOT_DIRECTORY);
  CharString out = newCharString();
  CharString f = newCharStringWithCString(TEST_FILENAME);
  CharString expected = newCharString();
  snprintf(expected->data, expected->length, "%s%c%s", ROOT_DIRECTORY, PATH_DELIMITER, TEST_FILENAME);
  buildAbsolutePath(d, f, "txt", out);
  assertCharStringEquals(out, expected->data);
  return 0;
}

static int _testConvertRelativePathToAbsolute(void) {
  return 0;
}

static int _testConvertRelativePathToAbsoluteAlreadyAbsolute(void) {
  return 0;
}

static int _testConvertRelativePathToAbsoluteEmpty(void) {
  return 0;
}

static int _testIsAbsolutePath(void) {
  assert(isAbsolutePath(newCharStringWithCString(ABSOLUTE_TEST_FILENAME)));
  return 0;
}

static int _testIsAbsolutePathUNCWindows(void) {
#if WINDOWS
  assert(isAbsolutePath(newCharStringWithCString("\\\\SERVER\\file")));
#endif
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
  return 0;
}

static int _testGetNullFileDirname(void) {
  CharString result = newCharString();
  getFileDirname(NULL, result);
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

  addTest(testSuite, "BuildAbsolutePath", NULL); // _testBuildAbsolutePath);
  addTest(testSuite, "BuildAbsolutePathWithFileExtension", NULL); // _testBuildAbsolutePathWithFileExtension);
  addTest(testSuite, "BuildAbsolutePathEmptyPath", NULL); // _testBuildAbsolutePathEmptyPath);
  addTest(testSuite, "BuildAbsolutePathEmptyFile", NULL); // _testBuildAbsolutePathEmptyFile);
  addTest(testSuite, "BuildAbsolutePathWithFileExtensionTwice", _testBuildAbsolutePathWithFileExtensionTwice);
  addTest(testSuite, "ConvertRelativePathToAbsolute", NULL); // _testConvertRelativePathToAbsolute);
  addTest(testSuite, "ConvertRelativePathToAbsoluteAlreadyAbsolute", NULL); // _testConvertRelativePathToAbsoluteAlreadyAbsolute);
  addTest(testSuite, "ConvertRelativePathToAbsoluteEmpty", NULL); // _testConvertRelativePathToAbsoluteEmpty);
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
