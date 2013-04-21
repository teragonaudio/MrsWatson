#if USE_NEW_FILE_API

#include "unit/TestRunner.h"
#include "base/File.h"

static int _testNewFileDefault(void) {
  File f = newFile();
  assertNotNull(f);
  assertCharStringEquals(f->absolutePath, "");
  assertIntEquals(f->fileType, kFileTypeInvalid);
  return 0;
}

static int _testNewFileWithRelativePath(void) {
  CharString p = newCharString();
  sprintf(p->data, "%s%c%s", "test", PATH_DELIMITER, "file");
  CharString pAbs = newCharString();
  sprintf(p->data, "%s%s%c%s%c%s", ROOT_DIRECTORY, get);
  return 0;
}

static int _testNewFileWithAbsolutePath(void) {
  CharString p = newCharString();
  sprintf(p->data, "%s%c%s", "test", PATH_DELIMITER, "file");
  
  return 0;
}

static int _testNewFileWithInvalidPath(void) {
  return 0;
}

static int _testNewFileWithNullPath(void) {
  return 0;
}

static int _testFileCreateFile(void) {
  return 0;
}

static int _testFileCreateDir(void) {
  return 0;
}

static int _testFileCreateNull(void) {
  return 0;
}

static int _testFileCreateAlreadyExists(void) {
  return 0;
}

static int _testFileCopyTo(void) {
  return 0;
}

static int _testFileCopyToInvalidDestination(void) {
  return 0;
}

static int _testFileCopyNullTo(void) {
  return 0;
}

static int _testFileCopyToNull(void) {
  return 0;
}

static int _testFileListContents(void) {
  return 0;
}

static int _testFileListDirectory(void) {
  return 0;
}

static int _testFileListContentsNull(void) {
  return 0;
}

static int _testFileListContentsInvalidDir(void) {
  return 0;
}

static int _testFileListContentsFile(void) {
  return 0;
}

static int _testFileListContentsEmptyDir(void) {
  return 0;
}

static int _testFileRemoveDir(void) {
  return 0;
}

static int _testFileRemoveFile(void) {
  return 0;
}

static int _testFileRemoveInvalid(void) {
  return 0;
}

static int _testFileRemoveNull(void) {
  return 0;
}

static int _testFileGetBasename(void) {
  return 0;
}

static int _testFileGetBasenameNull(void) {
  return 0;
}

static int _testFileGetBasenameInvalid(void) {
  return 0;
}

static int _testFileGetDirname(void) {
  return 0;
}

static int _testFileGetDirnameNull(void) {
  return 0;
}

static int _testFileGetDirnameInvalid(void) {
  return 0;
}

static int _testFileGetExtension(void) {
  return 0;
}

static int _testFileGetExtensionNull(void) {
  return 0;
}

static int _testFileGetExtensionInvalid(void) {
  return 0;
}

static int _testFileExists(void) {
  return 0;
}

static int _testFileExistsNull(void) {
  return 0;
}

static int _testFileExistsInvalid(void) {
  return 0;
}

TestSuite addFileTests(void);
TestSuite addFileTests(void) {
  TestSuite testSuite = newTestSuite("File", NULL, NULL);
  addTest(testSuite, "NewFileDefault", _testNewFileDefault);
  addTest(testSuite, "NewFileWithRelativePath", _testNewFileWithPath);
  addTest(testSuite, "NewFileWithAbsolutePath", _testNewFileWithPath);
  addTest(testSuite, "NewFileWithInvalidPath", _testNewFileWithInvalidPath);
  addTest(testSuite, "NewFileWithNullPath", _testNewFileWithNullPath);
  addTest(testSuite, "FileCreateFile", _testFileCreateFile);
  addTest(testSuite, "FileCreateDir", _testFileCreateDir);
  addTest(testSuite, "FileCreateNull", _testFileCreateNull);
  addTest(testSuite, "FileCreateAlreadyExists", _testFileCreateAlreadyExists);
  addTest(testSuite, "FileCopyTo", _testFileCopyTo);
  addTest(testSuite, "FileCopyToInvalidDestination", _testFileCopyToInvalidDestination);
  addTest(testSuite, "FileCopyNullTo", _testFileCopyNullTo);
  addTest(testSuite, "FileCopyToNull", _testFileCopyToNull);
  addTest(testSuite, "FileListContents", _testFileListContents);
  addTest(testSuite, "FileListDirectory", _testFileListDirectory);
  addTest(testSuite, "FileListContentsNull", _testFileListContentsNull);
  addTest(testSuite, "FileListContentsInvalidDir", _testFileListContentsInvalidDir);
  addTest(testSuite, "FileListContentsFile", _testFileListContentsFile);
  addTest(testSuite, "FileListContentsEmptyDir", _testFileListContentsEmptyDir);
  addTest(testSuite, "FileRemoveDir", _testFileRemoveDir);
  addTest(testSuite, "FileRemoveFile", _testFileRemoveFile);
  addTest(testSuite, "FileRemoveInvalid", _testFileRemoveInvalid);
  addTest(testSuite, "FileRemoveNull", _testFileRemoveNull);

  addTest(testSuite, "FileGetBasename", _testFileGetBasename);
  addTest(testSuite, "FileGetBasenameNull", _testFileGetBasenameNull);
  addTest(testSuite, "FileGetBasenameInvalid", _testFileGetBasenameInvalid);
  addTest(testSuite, "FileGetDirname", _testFileGetDirname);
  addTest(testSuite, "FileGetDirnameNull", _testFileGetDirnameNull);
  addTest(testSuite, "FileGetDirnameInvalid", _testFileGetDirnameInvalid);
  addTest(testSuite, "FileGetExtension", _testFileGetExtension);
  addTest(testSuite, "FileGetExtensionNull", _testFileGetExtensionNull);
  addTest(testSuite, "FileGetExtensionInvalid", _testFileGetExtensionInvalid);

  addTest(testSuite, "FileExists", _testFileExists);
  addTest(testSuite, "FileExistsNull", _testFileExistsNull);
  addTest(testSuite, "FileExistsInvalid", _testFileExistsInvalid);
  return testSuite;
}

#endif

