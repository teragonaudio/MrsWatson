#include "TestRunner.h"
#include "PlatformUtilities.h"

static int _testGetPlatformType(void) {
  return 0;
}

static int _testGetPlatformName(void) {
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

static int _testIsHostLittleEndian(void) {
  return 0;
}

static int _testFlipShortEndian(void) {
  return 0;
}

static int _testConvertBigEndianShortToPlatform(void) {
  return 0;
}

static int _testConvertBigEndianIntToPlatform(void) {
  return 0;
}

static int _testConvertLittleEndianIntToPlatform(void) {
  return 0;
}

static int _testConvertBigEndianFloatToPlatform(void) {
  return 0;
}

static int _testConvertByteArrayToUnsignedShort(void) {
  return 0;
}

static int _testConvertByteArrayToUnsignedInt(void) {
  return 0;
}

TestSuite addPlatformUtilitiesTests(void);
TestSuite addPlatformUtilitiesTests(void) {
  TestSuite testSuite = newTestSuite("PlatformUtilities", NULL, NULL);
  addTest(testSuite, "GetPlatformType", NULL); // _testGetPlatformType);
  addTest(testSuite, "GetPlatformName", NULL); // _testGetPlatformName);
  addTest(testSuite, "FileExists", NULL); // _testFileExists);
  addTest(testSuite, "FileExistsNull", NULL); // _testFileExistsNull);
  addTest(testSuite, "FileExistsInvalid", NULL); // _testFileExistsInvalid);

  addTest(testSuite, "IsHostLittleEndian", NULL); // _testIsHostLittleEndian);

  addTest(testSuite, "FlipShortEndian", NULL); // _testFlipShortEndian);
  addTest(testSuite, "ConvertBigEndianShortToPlatform", NULL); // _testConvertBigEndianShortToPlatform);
  addTest(testSuite, "ConvertBigEndianIntToPlatform", NULL); // _testConvertBigEndianIntToPlatform);
  addTest(testSuite, "ConvertLittleEndianIntToPlatform", NULL); // _testConvertLittleEndianIntToPlatform);
  addTest(testSuite, "ConvertBigEndianFloatToPlatform", NULL); // _testConvertBigEndianFloatToPlatform);

  addTest(testSuite, "ConvertByteArrayToUnsignedShort", NULL); // _testConvertByteArrayToUnsignedShort);
  addTest(testSuite, "ConvertByteArrayToUnsignedInt", NULL); // _testConvertByteArrayToUnsignedInt);
  return testSuite;
}
