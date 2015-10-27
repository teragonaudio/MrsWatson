#include "unit/TestRunner.h"
#include "base/File.h"

#define TEST_DIRNAME "test_dir"
#define TEST_DIRNAME_WITH_DOT "test.dir"
#define TEST_DIRNAME_COPY_DEST "test_dir_dest"
#define TEST_FILENAME "test_file.txt"

static void _fileTestTeardown(void)
{
    CharString testFilePath = newCharStringWithCString(TEST_FILENAME);
    File testFile = newFileWithPath(testFilePath);
    CharString testDirPath = newCharStringWithCString(TEST_DIRNAME);
    File testDir = newFileWithPath(testDirPath);
    CharString testDirWithDotPath = newCharStringWithCString(TEST_DIRNAME_WITH_DOT);
    File testDirWithDot = newFileWithPath(testDirWithDotPath);
    CharString testDirCopyPath = newCharStringWithCString(TEST_DIRNAME_COPY_DEST);
    File testDirCopy = newFileWithPath(testDirCopyPath);

    if (fileExists(testFile)) {
        fileRemove(testFile);
    }

    if (fileExists(testDir)) {
        fileRemove(testDir);
    }

    if (fileExists(testDirWithDot)) {
        fileRemove(testDirWithDot);
    }

    if (fileExists(testDirCopy)) {
        fileRemove(testDirCopy);
    }

    freeFile(testFile);
    freeFile(testDir);
    freeFile(testDirWithDot);
    freeFile(testDirCopy);
    freeCharString(testFilePath);
    freeCharString(testDirPath);
    freeCharString(testDirWithDotPath);
    freeCharString(testDirCopyPath);
}

static int _testNewFileDefault(void)
{
    File f = newFile();

    assertNotNull(f);
    assertCharStringEquals(EMPTY_STRING, f->absolutePath);
    assertIntEquals(kFileTypeInvalid, f->fileType);
    assertIntEquals(kFileOpenModeClosed, f->_openMode);
    assertIsNull(f->_fileHandle);
    assertFalse(fileExists(f));

    freeFile(f);
    return 0;
}

static int _testNewFileAlreadyExists(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    File ftest = NULL;

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));

    ftest = newFileWithPath(p);
    assertIntEquals(kFileTypeFile, ftest->fileType);
    assertIntEquals(kFileOpenModeClosed, ftest->_openMode);
    assertIsNull(ftest->_fileHandle);

    freeCharString(p);
    freeFile(f);
    freeFile(ftest);
    return 0;
}

static int _testNewFileWithRelativePath(void)
{
    CharString p = newCharString();
    CharString pAbs = newCharString();
    CharString pwd = fileGetCurrentDirectory();
    File f = NULL;

    sprintf(p->data, "%s%c%s", TEST_DIRNAME, PATH_DELIMITER, TEST_FILENAME);
    f = newFileWithPath(p);
    assertNotNull(f);
    sprintf(pAbs->data, "%s%c%s", pwd->data, PATH_DELIMITER, p->data);
    assertCharStringEquals(pAbs->data, f->absolutePath);
    assertFalse(fileExists(f));
    assertIntEquals(kFileTypeInvalid, f->fileType);
    assertIntEquals(kFileOpenModeClosed, f->_openMode);
    assertIsNull(f->_fileHandle);

    freeFile(f);
    freeCharString(p);
    freeCharString(pAbs);
    freeCharString(pwd);
    return 0;
}

static int _testNewFileWithAbsolutePath(void)
{
    CharString p = newCharString();
    CharString pAbs = newCharString();
    File f = NULL;

    sprintf(p->data, "%s%c%s%c%s", ROOT_DIRECTORY, PATH_DELIMITER, TEST_DIRNAME, PATH_DELIMITER, TEST_FILENAME);
    f = newFileWithPath(p);
    assertNotNull(f);
    // Mostly just testing to make sure that absolute paths are not incorrectly
    // translated from relative ones
    sprintf(pAbs->data, "%s%c%s%c%s", ROOT_DIRECTORY, PATH_DELIMITER, TEST_DIRNAME, PATH_DELIMITER, TEST_FILENAME);
    assert(charStringIsEqualTo(pAbs, f->absolutePath, false));
    assertFalse(fileExists(f));
    assertIntEquals(kFileTypeInvalid, f->fileType);
    assertIntEquals(kFileOpenModeClosed, f->_openMode);
    assertIsNull(f->_fileHandle);

    freeFile(f);
    freeCharString(p);
    freeCharString(pAbs);
    return 0;
}

static int _testNewFileWithNetworkPath(void)
{
#if WINDOWS
    CharString p = newCharString();
    CharString pAbs = newCharString();
    CharString pwd = fileGetCurrentDirectory();
    File f;

    sprintf(p->data, "\\\\%s%c%s", TEST_DIRNAME, PATH_DELIMITER, TEST_FILENAME);
    f = newFileWithPath(p);
    assertNotNull(f);
    // Mostly just testing to make sure that network paths are not incorrectly
    // translated from relative ones
    sprintf(pAbs->data, "\\\\%s%c%s", TEST_DIRNAME, PATH_DELIMITER, TEST_FILENAME);
    assert(charStringIsEqualTo(pAbs, f->absolutePath, false));
    assertFalse(fileExists(f));
    assertIntEquals(kFileTypeInvalid, f->fileType);
    assertIntEquals(kFileOpenModeClosed, f->_openMode);
    assertIsNull(f->_fileHandle);

    freeFile(f);
    freeCharString(p);
    freeCharString(pAbs);
    freeCharString(pwd);
#elif UNIX
    // Unix systems mount network drives as regular filesystem paths, this test
    // isn't necessary on that platform
#else
    assert(false);
#endif
    return 0;
}

static int _testNewFileWithInvalidPath(void)
{
    CharString p;
    File f;

    // These characters are not permitted in filenames on Windows or Unix. The
    // file interface should refuse to create an object with such a path,
    // regardless of type.

    p = newCharStringWithCString("*");
    f = newFileWithPath(p);
    assertIsNull(f);
    freeCharString(p);

    p = newCharStringWithCString(":");
    f = newFileWithPath(p);
    assertIsNull(f);
    freeCharString(p);

    p = newCharStringWithCString("?");
    f = newFileWithPath(p);
    assertIsNull(f);
    freeCharString(p);

    p = newCharStringWithCString("<");
    f = newFileWithPath(p);
    assertIsNull(f);
    freeCharString(p);

    p = newCharStringWithCString(">");
    f = newFileWithPath(p);
    assertIsNull(f);
    freeCharString(p);

    p = newCharStringWithCString("|");
    f = newFileWithPath(p);
    assertIsNull(f);
    freeCharString(p);

    freeFile(f);
    return 0;
}

static int _testNewFileWithNullPath(void)
{
    File f;

    // Should yield the same result as newFile()
    f = newFileWithPath(NULL);
    assertNotNull(f);
    assertCharStringEquals(EMPTY_STRING, f->absolutePath);
    assertIntEquals(kFileTypeInvalid, f->fileType);
    assertFalse(fileExists(f));

    freeFile(f);
    return 0;
}

static int _testNewFileWithCStringPath(void)
{
    File f = newFileWithPathCString(TEST_FILENAME);
    assertNotNull(f);
    assertFalse(fileExists(f));
    freeFile(f);
    return 0;
}

static int _testNewFileWithCStringPathNull(void)
{
    File f = newFileWithPathCString(NULL);
    assertIsNull(f);
    return 0;
}

static int _testNewFileWithParent(void)
{
    CharString pdir = newCharStringWithCString(TEST_DIRNAME);
    CharString pfile = newCharStringWithCString(TEST_FILENAME);
    File dir = newFileWithPath(pdir);
    File f = NULL;
    CharString pwd = fileGetCurrentDirectory();
    CharString pAbs = newCharString();

    assertFalse(fileExists(dir));
    assert(fileCreate(dir, kFileTypeDirectory));
    f = newFileWithParent(dir, pfile);
    assertNotNull(f);
    assertFalse(fileExists(f));
    sprintf(pAbs->data, "%s%c%s%c%s", pwd->data, PATH_DELIMITER, pdir->data, PATH_DELIMITER, pfile->data);
    assertCharStringEquals(pAbs->data, f->absolutePath);
    assertIntEquals(kFileOpenModeClosed, f->_openMode);
    assertIsNull(f->_fileHandle);

    freeCharString(pAbs);
    freeCharString(pdir);
    freeCharString(pfile);
    freeCharString(pwd);
    freeFile(f);
    freeFile(dir);
    return 0;
}

static int _testNewFileWithParentNullParent(void)
{
    CharString pfile = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithParent(NULL, pfile);
    assertIsNull(f);
    freeCharString(pfile);
    return 0;
}

static int _testNewFileWithParentNullPath(void)
{
    CharString pdir = newCharStringWithCString(TEST_DIRNAME);
    File d = newFileWithPath(pdir);
    File f = NULL;

    assertFalse(fileExists(d));
    assert(fileCreate(d, kFileTypeDirectory));
    assert(fileExists(d));
    newFileWithParent(d, NULL);
    assertIsNull(f);

    freeCharString(pdir);
    freeFile(d);
    return 0;
}

static int _testNewFileWithParentEmptyPath(void)
{
    CharString pdir = newCharStringWithCString(TEST_DIRNAME);
    CharString empty = newCharString();
    File d = newFileWithPath(pdir);
    File f = NULL;

    assertFalse(fileExists(d));
    assert(fileCreate(d, kFileTypeDirectory));
    assert(fileExists(d));
    newFileWithParent(d, empty);
    assertIsNull(f);

    freeCharString(pdir);
    freeCharString(empty);
    freeFile(d);
    return 0;
}

static int _testNewFileWithParentAlreadyExists(void)
{
    CharString pdir = newCharStringWithCString(TEST_DIRNAME);
    CharString pfile = newCharStringWithCString(TEST_FILENAME);
    File dir = newFileWithPath(pdir);
    File f = NULL;
    File ftest = NULL;

    assertFalse(fileExists(dir));
    assert(fileCreate(dir, kFileTypeDirectory));
    f = newFileWithParent(dir, pfile);
    assertNotNull(f);
    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    ftest = newFileWithParent(dir, pfile);
    assert(fileExists(ftest));
    assertIntEquals(kFileTypeFile, ftest->fileType);

    freeCharString(pdir);
    freeCharString(pfile);
    freeFile(f);
    freeFile(ftest);
    freeFile(dir);
    return 0;
}

static int _testNewFileWithParentNotDirectory(void)
{
    CharString pfile = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(pfile);
    File ftest = NULL;

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    ftest = newFileWithParent(f, pfile);
    assertIsNull(ftest);

    freeCharString(pfile);
    freeFile(f);
    freeFile(ftest);
    return 0;
}

static int _testNewFileWithParentInvalid(void)
{
    CharString pdir = newCharStringWithCString(TEST_DIRNAME);
    CharString pfile = newCharStringWithCString(TEST_FILENAME);
    File dir = newFileWithPath(pdir);
    File f = NULL;

    assertFalse(fileExists(dir));
    f = newFileWithParent(dir, pfile);
    assertIsNull(f);

    freeCharString(pdir);
    freeCharString(pfile);
    freeFile(f);
    freeFile(dir);
    return 0;
}

static int _testNewFileWithParentAbsolutePath(void)
{
    CharString pdir = newCharStringWithCString(TEST_DIRNAME);
    File dir = newFileWithPath(pdir);
    File f = NULL;
    CharString p = newCharString();

    assertFalse(fileExists(dir));
    assert(fileCreate(dir, kFileTypeDirectory));
    assert(fileExists(dir));
    sprintf(p->data, "%s%c%s", ROOT_DIRECTORY, PATH_DELIMITER, TEST_FILENAME);
    f = newFileWithParent(dir, p);
    assertIsNull(f);

    freeCharString(p);
    freeCharString(pdir);
    freeFile(f);
    freeFile(dir);
    return 0;
}

static int _testFileExists(void)
{
    File f;
    FILE *fp = fopen(TEST_FILENAME, "w");
    CharString c = newCharStringWithCString(TEST_FILENAME);
    assert(fp != NULL);
    fclose(fp);

    f = newFileWithPath(c);
    assert(fileExists(f));

    freeFile(f);
    freeCharString(c);
    return 0;
}

static int _testFileExistsInvalid(void)
{
    CharString c = newCharStringWithCString("invalid");
    File f = newFileWithPath(c);

    assertFalse(fileExists(f));

    freeFile(f);
    freeCharString(c);
    return 0;
}

static int _testFileCreateFile(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f, f2;

    f = newFileWithPath(p);
    assertNotNull(f);
    // Might fail because a previous test didn't clean up properly
    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));
    assertIntEquals(kFileOpenModeWrite, f->_openMode);
    assertNotNull(f->_fileHandle);

    // Just to make sure...
    f2 = newFileWithPath(p);
    assert(fileExists(f2));
    assertIntEquals(kFileTypeFile, f2->fileType);
    assertIntEquals(kFileOpenModeClosed, f2->_openMode);
    assertIsNull(f2->_fileHandle);

    freeFile(f);
    freeFile(f2);
    freeCharString(p);
    return 0;
}

static int _testFileCreateDir(void)
{
    CharString p = newCharStringWithCString(TEST_DIRNAME);
    File f = newFileWithPath(p);
    File f2;

    assertNotNull(f);
    // Might fail because a previous test didn't clean up properly
    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeDirectory));
    assert(fileExists(f));
    // Just to make sure...
    f2 = newFileWithPath(p);
    assert(fileExists(f2));
    assertIntEquals(kFileTypeDirectory, f2->fileType);
    assertIntEquals(kFileOpenModeClosed, f->_openMode);
    assertIsNull(f->_fileHandle);

    freeFile(f);
    freeFile(f2);
    freeCharString(p);
    return 0;
}

static int _testFileCreateInvalidType(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);

    assertNotNull(f);
    // Might fail because a previous test didn't clean up properly
    assertFalse(fileExists(f));
    assertFalse(fileCreate(f, kFileTypeInvalid));
    assertFalse(fileExists(f));

    freeFile(f);
    freeCharString(p);
    return 0;
}

static int _testFileCreateAlreadyExists(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);

    assertNotNull(f);
    // Might fail because a previous test didn't clean up properly
    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));
    assertFalse(fileCreate(f, kFileTypeFile));

    freeFile(f);
    freeCharString(p);
    return 0;
}

static int _testFileCopyToWithFile(void)
{
    CharString psrc = newCharStringWithCString(TEST_FILENAME);
    CharString pdest = newCharStringWithCString(TEST_DIRNAME);
    File src = newFileWithPath(psrc);
    File dest = newFileWithPath(pdest);
    File copy = NULL;

    assertNotNull(src);
    assertNotNull(dest);
    assertFalse(fileExists(src));
    assertFalse(fileExists(dest));

    assert(fileCreate(dest, kFileTypeDirectory));
    assert(fileExists(dest));
    assert(fileCreate(src, kFileTypeFile));
    assert(fileExists(src));
    copy = fileCopyTo(src, dest);
    assertNotNull(copy);
    assert(fileExists(copy));

    freeFile(src);
    freeFile(dest);
    freeFile(copy);
    freeCharString(psrc);
    freeCharString(pdest);
    return 0;
}

static int _testFileCopyToWithDirectory(void)
{
    CharString psrc = newCharStringWithCString(TEST_DIRNAME);
    CharString psrcFile = newCharStringWithCString(TEST_FILENAME);
    CharString pdest = newCharStringWithCString(TEST_DIRNAME_COPY_DEST);
    File src = newFileWithPath(psrc);
    File srcFile = NULL;
    File dest = newFileWithPath(pdest);
    File copy = NULL;

    assertNotNull(src);
    assertNotNull(dest);
    assertFalse(fileExists(src));
    assertFalse(fileExists(dest));

    assert(fileCreate(dest, kFileTypeDirectory));
    assert(fileExists(dest));

    assert(fileCreate(src, kFileTypeDirectory));
    assert(fileExists(src));
    srcFile = newFileWithParent(src, psrcFile);
    assertNotNull(srcFile);
    assertFalse(fileExists(srcFile));
    assert(fileCreate(srcFile, kFileTypeFile));
    assert(fileExists(srcFile));

    copy = fileCopyTo(src, dest);
    assertNotNull(copy);
    assert(fileExists(copy));

    freeFile(src);
    freeFile(srcFile);
    freeFile(dest);
    freeFile(copy);
    freeCharString(psrc);
    freeCharString(psrcFile);
    freeCharString(pdest);
    return 0;
}

static int _testFileCopyToInvalidDestination(void)
{
    CharString psrc = newCharStringWithCString(TEST_FILENAME);
    CharString pdest = newCharStringWithCString("invalid");
    File src = newFileWithPath(psrc);
    File dest = newFileWithPath(pdest);
    File copy = NULL;

    assertNotNull(src);
    assertNotNull(dest);
    assertFalse(fileExists(src));
    assertFalse(fileExists(dest));

    assert(fileCreate(src, kFileTypeFile));
    assert(fileExists(src));
    copy = fileCopyTo(src, dest);
    assertIsNull(copy);

    freeFile(src);
    freeFile(dest);
    freeFile(copy);
    freeCharString(psrc);
    freeCharString(pdest);
    return 0;
}

static int _testFileCopyInvalidTo(void)
{
    CharString psrc = newCharStringWithCString("invalid");
    CharString pdest = newCharStringWithCString(TEST_DIRNAME);
    File src = newFileWithPath(psrc);
    File dest = newFileWithPath(pdest);
    File copy = NULL;

    assertNotNull(src);
    assertNotNull(dest);
    assertFalse(fileExists(src));
    assertFalse(fileExists(dest));

    assert(fileCreate(dest, kFileTypeDirectory));
    assert(fileExists(dest));
    copy = fileCopyTo(src, dest);
    assertIsNull(copy);

    freeFile(src);
    freeFile(dest);
    freeFile(copy);
    freeCharString(psrc);
    freeCharString(pdest);
    return 0;
}

static int _testFileCopyToNull(void)
{
    CharString psrc = newCharStringWithCString(TEST_FILENAME);
    File src = newFileWithPath(psrc);
    File copy = NULL;

    assertNotNull(src);
    assertFalse(fileExists(src));

    assert(fileCreate(src, kFileTypeFile));
    assert(fileExists(src));
    copy = fileCopyTo(src, NULL);
    assertIsNull(copy);

    freeFile(src);
    freeFile(copy);
    freeCharString(psrc);
    return 0;
}

static int _testFileRemoveDir(void)
{
    CharString pdest = newCharStringWithCString(TEST_DIRNAME);
    File dir = newFileWithPath(pdest);

    assertFalse(fileExists(dir));
    assert(fileCreate(dir, kFileTypeDirectory));
    assert(fileExists(dir));
    assert(fileRemove(dir));
    assertFalse(fileExists(dir));

    freeFile(dir);
    freeCharString(pdest);
    return 0;
}

static int _testFileRemoveDirWithContents(void)
{
    CharString pdest = newCharStringWithCString(TEST_DIRNAME);
    CharString psubdir = newCharStringWithCString(TEST_DIRNAME);
    CharString psubfile = newCharStringWithCString(TEST_FILENAME);
    File dir = newFileWithPath(pdest);
    File subdir = NULL;
    File subfile = NULL;

    assertFalse(fileExists(dir));
    assert(fileCreate(dir, kFileTypeDirectory));
    assert(fileExists(dir));

    // Must be created after parent directory is created
    subdir = newFileWithParent(dir, psubdir);
    assertNotNull(subdir);
    assertFalse(fileExists(subdir));
    assert(fileCreate(subdir, kFileTypeDirectory));
    assert(fileExists(subdir));

    // Must be created after parent directory is created
    subfile = newFileWithParent(dir, psubfile);
    assertNotNull(subfile);
    assertFalse(fileExists(subfile));
    assert(fileCreate(subfile, kFileTypeFile));
    assert(fileExists(subfile));
    // Windows will not be able to delete this directory unless all files in it are
    // closed and unlocked.
    fileClose(subfile);

    // Remove the parent directory and assert that all children are gone
    assert(fileRemove(dir));
    assertFalse(fileExists(dir));
    assertFalse(fileExists(subdir));
    assertFalse(fileExists(subfile));

    freeFile(dir);
    freeFile(subdir);
    freeFile(subfile);
    freeCharString(pdest);
    freeCharString(psubdir);
    freeCharString(psubfile);
    return 0;
}

static int _testFileRemoveFile(void)
{
    CharString pfile = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(pfile);

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));
    assert(fileRemove(f));
    assertFalse(fileExists(f));

    freeCharString(pfile);
    freeFile(f);
    return 0;
}

static int _testFileRemoveInvalid(void)
{
    CharString pfile = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(pfile);

    assertFalse(fileExists(f));
    assertFalse(fileRemove(f));

    freeCharString(pfile);
    freeFile(f);
    return 0;
}

static int _testFileListDirectory(void)
{
    CharString pdest = newCharStringWithCString(TEST_DIRNAME);
    CharString psubdir = newCharStringWithCString(TEST_DIRNAME);
    CharString psubfile = newCharStringWithCString(TEST_FILENAME);
    File dir = newFileWithPath(pdest);
    File subdir = NULL;
    File subfile = NULL;
    LinkedList list;
    File item;

    assertFalse(fileExists(dir));
    assert(fileCreate(dir, kFileTypeDirectory));
    assert(fileExists(dir));

    subdir = newFileWithParent(dir, psubdir);
    assertNotNull(subdir);
    assertFalse(fileExists(subdir));
    assert(fileCreate(subdir, kFileTypeDirectory));
    assertNotNull(subdir);

    subfile = newFileWithParent(dir, psubfile);
    assertNotNull(subfile);
    assertFalse(fileExists(subfile));
    assert(fileCreate(subfile, kFileTypeFile));
    assert(fileExists(subfile));

    list = fileListDirectory(dir);
    assertNotNull(list);
    item = (File)list->item;
    assertNotNull(item);

    if (item->fileType == kFileTypeFile) {
        assertCharStringContains(TEST_FILENAME, item->absolutePath);
    } else if (item->fileType == kFileTypeDirectory) {
        assertCharStringContains(TEST_DIRNAME, item->absolutePath);
    } else {
        assert(false);
    }

    item = (File)(((LinkedList)list->nextItem)->item);
    assertNotNull(item);

    if (item->fileType == kFileTypeFile) {
        assertCharStringContains(TEST_FILENAME, item->absolutePath);
    } else if (item->fileType == kFileTypeDirectory) {
        assertCharStringContains(TEST_DIRNAME, item->absolutePath);
    } else {
        assert(false);
    }

    freeLinkedListAndItems(list, (LinkedListFreeItemFunc)freeFile);
    freeFile(dir);
    freeFile(subfile);
    freeFile(subdir);
    freeCharString(pdest);
    freeCharString(psubdir);
    freeCharString(psubfile);
    return 0;
}

static int _testFileListDirectoryInvalid(void)
{
    CharString pdest = newCharStringWithCString(TEST_FILENAME);
    File file = newFileWithPath(pdest);
    LinkedList list;

    assertFalse(fileExists(file));
    assert(fileCreate(file, kFileTypeFile));
    assert(fileExists(file));
    list = fileListDirectory(file);
    assertIsNull(list);

    freeFile(file);
    freeCharString(pdest);
    return 0;
}

static int _testFileListDirectoryNotExists(void)
{
    CharString pdest = newCharStringWithCString(TEST_DIRNAME);
    File dir = newFileWithPath(pdest);
    LinkedList list;

    assertFalse(fileExists(dir));
    list = fileListDirectory(dir);
    assertIsNull(list);

    freeFile(dir);
    freeCharString(pdest);
    return 0;
}

static int _testFileListDirectoryEmpty(void)
{
    CharString pdest = newCharStringWithCString(TEST_DIRNAME);
    File dir = newFileWithPath(pdest);
    LinkedList list;

    assertFalse(fileExists(dir));
    assert(fileCreate(dir, kFileTypeDirectory));
    assert(fileExists(dir));
    list = fileListDirectory(dir);
    assertNotNull(list);
    assertIntEquals(0, linkedListLength(list));

    freeLinkedList(list);
    freeFile(dir);
    freeCharString(pdest);
    return 0;
}

static int _testFileGetSize(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileWrite(f, p));
    fileClose(f); // force flush and close to be called
    assertSizeEquals(strlen(p->data), fileGetSize(f));

    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileGetSizeNotExists(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);

    assertFalse(fileExists(f));
    assertSizeEquals((size_t)0, fileGetSize(f));

    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileGetSizeDirectory(void)
{
    CharString p = newCharStringWithCString(TEST_DIRNAME);
    File d = newFileWithPath(p);

    assertFalse(fileExists(d));
    assert(fileCreate(d, kFileTypeDirectory));
    assert(fileExists(d));
    assertSizeEquals((size_t)0, fileGetSize(d));

    freeCharString(p);
    freeFile(d);
    return 0;
}

static int _testFileReadContents(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    CharString result = NULL;

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));
    assert(fileWrite(f, p));
    result = fileReadContents(f);
    assertNotNull(result);
    assertCharStringEquals(p->data, result);

    freeCharString(result);
    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileReadContentsNotExists(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    CharString result = NULL;

    assertFalse(fileExists(f));
    result = fileReadContents(f);
    assertIsNull(result);

    freeCharString(result);
    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileReadContentsDirectory(void)
{
    CharString p = newCharStringWithCString(TEST_DIRNAME);
    File d = newFileWithPath(p);
    CharString result = NULL;

    assertFalse(fileExists(d));
    assert(fileCreate(d, kFileTypeDirectory));
    assert(fileExists(d));
    result = fileReadContents(d);
    assertIsNull(result);

    freeCharString(result);
    freeCharString(p);
    freeFile(d);
    return 0;
}

static int _testFileReadLines(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    LinkedList lines = NULL;
    CharString *items = NULL;
    int i;

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));
    assert(fileWrite(f, p));
    assert(fileWriteBytes(f, "\n", 1));
    assert(fileWrite(f, p));
    lines = fileReadLines(f);
    assertNotNull(lines);
    assertIntEquals(2, linkedListLength(lines));
    items = (CharString *)linkedListToArray(lines);
    assertNotNull(items);

    for (i = 0; i < linkedListLength(lines); i++) {
        assertCharStringEquals(p->data, items[i]);
    }

    freeLinkedListAndItems(lines, (LinkedListFreeItemFunc)freeCharString);
    freeCharString(p);
    freeFile(f);
    free(items);
    return 0;
}

static int _testFileReadLinesEmpty(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    LinkedList lines = NULL;

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));
    lines = fileReadLines(f);
    assertNotNull(lines);
    assertIntEquals(0, linkedListLength(lines));

    freeLinkedList(lines);
    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileReadLinesNotExists(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    LinkedList result = NULL;

    assertFalse(fileExists(f));
    result = fileReadLines(f);
    assertIsNull(result);

    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileReadLinesDirectory(void)
{
    CharString p = newCharStringWithCString(TEST_DIRNAME);
    File d = newFileWithPath(p);
    LinkedList result = NULL;

    assertFalse(fileExists(d));
    assert(fileCreate(d, kFileTypeDirectory));
    assert(fileExists(d));
    result = fileReadLines(d);
    assertIsNull(result);

    freeCharString(p);
    freeFile(d);
    return 0;
}

static int _testFileReadBytes(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    size_t s = 0;
    CharString result = NULL;

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));
    assert(fileWrite(f, p));
    fileClose(f);
    s = fileGetSize(f);
    assert(s > 0);
    char *fileData = fileReadBytes(f, s);
    result = newCharStringWithCString(fileData);
    assertNotNull(result);
    assertCharStringEquals(TEST_FILENAME, result);

    freeCharString(result);
    freeCharString(p);
    freeFile(f);
    free(fileData);
    return 0;
}

static int _testFileReadBytesNotExists(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    void *result = NULL;

    assertFalse(fileExists(f));
    // Note the fake size here
    result = fileReadBytes(f, (size_t)100);
    assertIsNull(result);

    freeCharString(result);
    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileReadBytesDirectory(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File d = newFileWithPath(p);
    void *result = NULL;

    assertFalse(fileExists(d));
    assert(fileCreate(d, kFileTypeDirectory));
    assert(fileExists(d));
    result = fileReadBytes(d, (size_t)100);
    assertIsNull(result);

    freeCharString(result);
    freeCharString(p);
    freeFile(d);
    return 0;
}

static int _testFileReadBytesZeroSize(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    size_t s = 0;
    char *result = NULL;

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));
    assert(fileWrite(f, p));
    fileClose(f);
    s = fileGetSize(f);
    assert(s > 0);
    result = (char *)fileReadBytes(f, 0);
    assertIsNull(result);

    free(result);
    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileReadBytesGreaterSize(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    size_t s = 0;
    CharString result = NULL;

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));
    assert(fileWrite(f, p));
    fileClose(f);
    s = fileGetSize(f);
    assert(s > 0);
    char *fileData = (char *)fileReadBytes(f, s * 2);
    result = newCharStringWithCString(fileData);
    assertNotNull(result);
    assertCharStringEquals(TEST_FILENAME, result);

    freeCharString(result);
    freeCharString(p);
    freeFile(f);
    free(fileData);
    return 0;
}

static int _testFileWrite(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    CharString result = NULL;

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));
    assert(fileWrite(f, p));
    result = fileReadContents(f);
    assertNotNull(result);
    assertCharStringEquals(TEST_FILENAME, result);

    freeCharString(result);
    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileWriteMultiple(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    CharString p2 = newCharStringWithCString(p->data);
    File f = newFileWithPath(p);
    CharString result = NULL;

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));
    assert(fileWrite(f, p));
    assert(fileWrite(f, p));
    result = fileReadContents(f);
    assertNotNull(result);
    charStringAppend(p, p2);
    assertCharStringEquals(p->data, result);

    freeCharString(result);
    freeCharString(p);
    freeCharString(p2);
    freeFile(f);
    return 0;
}

static int _testFileWriteInvalid(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);

    assertFalse(fileExists(f));
    assertFalse(fileWrite(f, p));

    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileWriteDirectory(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File d = newFileWithPath(p);

    assertFalse(fileExists(d));
    assert(fileCreate(d, kFileTypeDirectory));
    assertFalse(fileWrite(d, p));

    freeCharString(p);
    freeFile(d);
    return 0;
}

static int _testFileWriteBytes(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    size_t s = 0;
    CharString result = NULL;

    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));
    assert(fileWriteBytes(f, p->data, strlen(p->data)));
    fileClose(f);
    s = fileGetSize(f);
    assert(s > 0);
    char *fileData = (char *)fileReadBytes(f, s);
    result = newCharStringWithCString(fileData);
    assertNotNull(result);
    assertCharStringEquals(TEST_FILENAME, result);

    freeCharString(result);
    freeCharString(p);
    freeFile(f);
    free(fileData);
    return 0;
}

static int _testFileWriteBytesInvalid(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);

    assertFalse(fileExists(f));
    assertFalse(fileWriteBytes(f, p->data, strlen(p->data)));

    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileWriteBytesDirectory(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File d = newFileWithPath(p);

    assertFalse(fileExists(d));
    assert(fileCreate(d, kFileTypeDirectory));
    assertFalse(fileWriteBytes(d, p->data, strlen(p->data)));

    freeCharString(p);
    freeFile(d);
    return 0;
}

static int _testFileGetBasename(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    CharString b = fileGetBasename(f);

    assertNotNull(b);
    assertCharStringEquals(TEST_FILENAME, b);

    freeCharString(b);
    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileGetBasenameInvalid(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFile();
    CharString b = fileGetBasename(f);

    assertIsNull(b);

    freeCharString(p);
    freeFile(f);
    return 0;
}

static int _testFileGetBasenameDirectory(void)
{
    CharString p = newCharStringWithCString(TEST_DIRNAME);
    File d = newFileWithPath(p);
    CharString b = NULL;

    assertFalse(fileExists(d));
    assert(fileCreate(d, kFileTypeDirectory));
    b = fileGetBasename(d);
    assertNotNull(b);
    assertCharStringEquals(TEST_DIRNAME, b);

    freeCharString(b);
    freeCharString(p);
    freeFile(d);
    return 0;
}

static int _testFileGetParent(void)
{
    CharString pdir = newCharStringWithCString(TEST_DIRNAME);
    File dir = newFileWithPath(pdir);
    CharString pfile = newCharStringWithCString(TEST_FILENAME);
    File f = NULL;
    File result = NULL;

    assertFalse(fileExists(dir));
    assert(fileCreate(dir, kFileTypeDirectory));
    assert(fileExists(dir));

    f = newFileWithParent(dir, pfile);
    assertNotNull(f);
    assertFalse(fileExists(f));
    assert(fileCreate(f, kFileTypeFile));
    assert(fileExists(f));

    result = fileGetParent(f);
    assertNotNull(result);
    assertCharStringEquals(dir->absolutePath->data, result->absolutePath);

    freeCharString(pdir);
    freeCharString(pfile);
    freeFile(f);
    freeFile(dir);
    freeFile(result);
    return 0;
}

static int _testFileGetParentInvalid(void)
{
    File f = newFile();
    File result = fileGetParent(f);
    assertIsNull(result);
    freeFile(f);
    return 0;
}

static int _testFileGetExtension(void)
{
    CharString p = newCharStringWithCString(TEST_FILENAME);
    File f = newFileWithPath(p);
    CharString extension = fileGetExtension(f);

    assertNotNull(extension);
    assertCharStringEquals("txt", extension);

    freeCharString(p);
    freeCharString(extension);
    freeFile(f);
    return 0;
}

static int _testFileGetExtensionDirectory(void)
{
    CharString p = newCharStringWithCString(TEST_DIRNAME);
    File d = newFileWithPath(p);
    CharString result = NULL;

    assertFalse(fileExists(d));
    assert(fileCreate(d, kFileTypeDirectory));
    assert(fileExists(d));
    result = fileGetExtension(d);
    assertIsNull(result);

    freeCharString(p);
    freeFile(d);
    return 0;
}

static int _testFileGetExtensionInvalid(void)
{
    File f = newFile();
    CharString result = fileGetExtension(f);
    assertIsNull(result);
    freeFile(f);
    return 0;
}

static int _testFileGetExtensionNone(void)
{
    CharString p = newCharStringWithCString(TEST_DIRNAME);
    File d = newFileWithPath(p);
    CharString result = NULL;

    result = fileGetExtension(d);
    assertIsNull(result);

    freeCharString(p);
    freeFile(d);
    return 0;
}

static int _testFileGetExtensionWithDotInPath(void)
{
    CharString p = newCharStringWithCString(TEST_DIRNAME_WITH_DOT);
    File d = newFileWithPath(p);
    CharString pfile = newCharStringWithCString(TEST_DIRNAME_COPY_DEST);
    File f = NULL;
    CharString result = NULL;

    assertFalse(fileExists(d));
    assert(fileCreate(d, kFileTypeDirectory));

    f = newFileWithParent(d, pfile);
    assertNotNull(f);
    result = fileGetExtension(f);
    // The parent directory has a dot, but the file doesn't so this call
    // should return null.
    assertIsNull(result);

    freeCharString(p);
    freeCharString(pfile);
    freeFile(f);
    freeFile(d);
    return 0;
}

static int _testFileFreeNull(void)
{
    freeFile(NULL);
    return 0;
}

TestSuite addFileTests(void);
TestSuite addFileTests(void)
{
    TestSuite testSuite = newTestSuite("File", NULL, _fileTestTeardown);
    addTest(testSuite, "NewFileDefault", _testNewFileDefault);

    addTest(testSuite, "NewFileAlreadyExists", _testNewFileAlreadyExists);
    addTest(testSuite, "NewFileWithRelativePath", _testNewFileWithRelativePath);
    addTest(testSuite, "NewFileWithAbsolutePath", _testNewFileWithAbsolutePath);
    addTest(testSuite, "NewFileWithNetworkPath", _testNewFileWithNetworkPath);
    addTest(testSuite, "NewFileWithInvalidPath", _testNewFileWithInvalidPath);
    addTest(testSuite, "NewFileWithNullPath", _testNewFileWithNullPath);

    addTest(testSuite, "NewFileWithCStringPath", _testNewFileWithCStringPath);
    addTest(testSuite, "NewFileWithCStringPathNull", _testNewFileWithCStringPathNull);

    addTest(testSuite, "NewFileWithParent", _testNewFileWithParent);
    addTest(testSuite, "NewFileWithParentNullParent", _testNewFileWithParentNullParent);
    addTest(testSuite, "NewFileWithParentNullPath", _testNewFileWithParentNullPath);
    addTest(testSuite, "NewFileWithParentEmptyPath", _testNewFileWithParentEmptyPath);
    addTest(testSuite, "NewFileWithParentAlreadyExists", _testNewFileWithParentAlreadyExists);
    addTest(testSuite, "NewFileWithParentNotDirectory", _testNewFileWithParentNotDirectory);
    addTest(testSuite, "NewFileWithParentInvalid", _testNewFileWithParentInvalid);
    addTest(testSuite, "NewFileWithParentAbsolutePath", _testNewFileWithParentAbsolutePath);

    addTest(testSuite, "FileExists", _testFileExists);
    addTest(testSuite, "FileExistsInvalid", _testFileExistsInvalid);

    addTest(testSuite, "FileCreateFile", _testFileCreateFile);
    addTest(testSuite, "FileCreateDir", _testFileCreateDir);
    addTest(testSuite, "FileCreateInvalidType", _testFileCreateInvalidType);
    addTest(testSuite, "FileCreateAlreadyExists", _testFileCreateAlreadyExists);

    addTest(testSuite, "FileCopyToWithFile", _testFileCopyToWithFile);
    addTest(testSuite, "FileCopyToWithDirectory", _testFileCopyToWithDirectory);
    addTest(testSuite, "FileCopyToInvalidDestination", _testFileCopyToInvalidDestination);
    addTest(testSuite, "FileCopyNullTo", _testFileCopyInvalidTo);
    addTest(testSuite, "FileCopyToNull", _testFileCopyToNull);

    addTest(testSuite, "FileRemoveDir", _testFileRemoveDir);
    addTest(testSuite, "FileRemoveDirWithContents", _testFileRemoveDirWithContents);
    addTest(testSuite, "FileRemoveFile", _testFileRemoveFile);
    addTest(testSuite, "FileRemoveInvalid", _testFileRemoveInvalid);

    addTest(testSuite, "FileListDirectory", _testFileListDirectory);
    addTest(testSuite, "FileListDirectoryInvalid", _testFileListDirectoryInvalid);
    addTest(testSuite, "FileListDirectoryNotExists", _testFileListDirectoryNotExists);
    addTest(testSuite, "FileListDirectoryEmpty", _testFileListDirectoryEmpty);

    addTest(testSuite, "FileGetSize", _testFileGetSize);
    addTest(testSuite, "FileGetSizeNotExists", _testFileGetSizeNotExists);
    addTest(testSuite, "FileGetSizeDirectory", _testFileGetSizeDirectory);

    addTest(testSuite, "FileReadContents", _testFileReadContents);
    addTest(testSuite, "FileReadContentsNotExists", _testFileReadContentsNotExists);
    addTest(testSuite, "FileReadContentsDirectory", _testFileReadContentsDirectory);

    addTest(testSuite, "FileReadLines", _testFileReadLines);
    addTest(testSuite, "FileReadLinesEmpty", _testFileReadLinesEmpty);
    addTest(testSuite, "FileReadLinesNotExists", _testFileReadLinesNotExists);
    addTest(testSuite, "FileReadLinesDirectory", _testFileReadLinesDirectory);

    addTest(testSuite, "FileReadBytes", _testFileReadBytes);
    addTest(testSuite, "FileReadBytesNotExists", _testFileReadBytesNotExists);
    addTest(testSuite, "FileReadBytesDirectory", _testFileReadBytesDirectory);
    addTest(testSuite, "FileReadBytesZeroSize", _testFileReadBytesZeroSize);
    addTest(testSuite, "FileReadBytesGreaterSize", _testFileReadBytesGreaterSize);

    addTest(testSuite, "FileWrite", _testFileWrite);
    addTest(testSuite, "FileWriteMulitple", _testFileWriteMultiple);
    addTest(testSuite, "FileWriteInvalid", _testFileWriteInvalid);
    addTest(testSuite, "FileWriteDirectory", _testFileWriteDirectory);

    addTest(testSuite, "FileWriteBytes", _testFileWriteBytes);
    addTest(testSuite, "FileWriteBytesInvalid", _testFileWriteBytesInvalid);
    addTest(testSuite, "FileWriteBytesDirectory", _testFileWriteBytesDirectory);

    addTest(testSuite, "FileGetBasename", _testFileGetBasename);
    addTest(testSuite, "FileGetBasenameInvalid", _testFileGetBasenameInvalid);
    addTest(testSuite, "FileGetBasenameDirectory", _testFileGetBasenameDirectory);

    addTest(testSuite, "FileGetParent", _testFileGetParent);
    addTest(testSuite, "FileGetParentInvalid", _testFileGetParentInvalid);

    addTest(testSuite, "FileGetExtension", _testFileGetExtension);
    addTest(testSuite, "FileGetExtensionDirectory", _testFileGetExtensionDirectory);
    addTest(testSuite, "FileGetExtensionInvalid", _testFileGetExtensionInvalid);
    addTest(testSuite, "FileGetExtensionNone", _testFileGetExtensionNone);
    addTest(testSuite, "FileGetExtensionWithDotInPath", _testFileGetExtensionWithDotInPath);

    addTest(testSuite, "FileFreeNull", _testFileFreeNull);

    return testSuite;
}
