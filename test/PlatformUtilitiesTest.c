#include "TestRunner.h"
#include "PlatformUtilities.h"

static int _testGetPlatformType(void) {
#if LINUX
  assertIntEquals(getPlatformType(), PLATFORM_LINUX);
#elif MACOSX
  assertIntEquals(getPlatformType(), PLATFORM_MACOSX);
#elif WINDOWS
  assertIntEquals(getPlatformType(), PLATFORM_WINDOWS);
#else
  assertIntEquals(getPlatformType(), PLATFORM_UNSUPPORTED);
#endif
  return 0;
}

static int _testGetPlatformName(void) {
  CharString p = getPlatformName();
  #if LINUX
  assertCharStringContains(p->data, "Linux");
#elif MACOSX
  assertCharStringContains(p->data, "Mac OSX");
#elif WINDOWS
  assertCharStringContains(p->data, "Windows");
#else
  assertCharStringEquals(p->data, "Unsupported platform");
#endif
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
  addTest(testSuite, "GetPlatformType", _testGetPlatformType);
  addTest(testSuite, "GetPlatformName", _testGetPlatformName);

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
