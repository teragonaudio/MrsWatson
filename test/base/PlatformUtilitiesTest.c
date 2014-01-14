#include "unit/TestRunner.h"
#include "base/PlatformUtilities.h"
#if MACOSX
#include <machine/endian.h>
#endif

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
  assertCharStringContains(p, "Linux");
#elif MACOSX
  assertCharStringContains(p, "Mac OS X");
#elif WINDOWS
  assertCharStringContains(p, "Windows");
#else
  assertCharStringEquals(p, "Unsupported platform");
#endif
  freeCharString(p);
  return 0;
}

static int _testGetShortPlatformName(void) {
  CharString p = newCharStringWithCString(getShortPlatformName());
#if LINUX
  if(isHost64Bit() && isExecutable64Bit()) {
    assertCharStringEquals(p, "Linux-x86_64");
  }
  else {
    assertCharStringEquals(p, "Linux-i686");
  }
#elif MACOSX
  assertCharStringEquals(p, "Mac OS X");
#elif WINDOWS
  if(isHost64Bit() && isExecutable64Bit()) {
    assertCharStringEquals(p, "Windows 64-bit");
  }
  else {
    assertCharStringEquals(p, "Windows 32-bit");
  }
#else
  assertCharStringEquals(p, "Unsupported");
#endif
  freeCharString(p);
  return 0;
}

static int _testIsHostLittleEndian(void) {
#if LINUX
#elif MACOSX
#if __DARWIN_BYTE_ORDER == __DARWIN_BIG_ENDIAN
  assertFalse(isHostLittleEndian());
#elif __DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN
  assert(isHostLittleEndian());
#endif
#elif WINDOWS
#endif
  return 0;
}

static int _testFlipShortEndian(void) {
  unsigned short s = 0xabcd;
  unsigned short r = flipShortEndian(s);
  assertUnsignedLongEquals(r, 0xcdab);
  assertIntEquals(s, flipShortEndian(r));
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
  addTest(testSuite, "GetShortPlatformName", _testGetShortPlatformName);

  addTest(testSuite, "IsHostLittleEndian", _testIsHostLittleEndian);

  addTest(testSuite, "FlipShortEndian", _testFlipShortEndian);
  addTest(testSuite, "ConvertBigEndianShortToPlatform", NULL); // _testConvertBigEndianShortToPlatform);
  addTest(testSuite, "ConvertBigEndianIntToPlatform", NULL); // _testConvertBigEndianIntToPlatform);
  addTest(testSuite, "ConvertLittleEndianIntToPlatform", NULL); // _testConvertLittleEndianIntToPlatform);
  addTest(testSuite, "ConvertBigEndianFloatToPlatform", NULL); // _testConvertBigEndianFloatToPlatform);

  addTest(testSuite, "ConvertByteArrayToUnsignedShort", NULL); // _testConvertByteArrayToUnsignedShort);
  addTest(testSuite, "ConvertByteArrayToUnsignedInt", NULL); // _testConvertByteArrayToUnsignedInt);
  return testSuite;
}
