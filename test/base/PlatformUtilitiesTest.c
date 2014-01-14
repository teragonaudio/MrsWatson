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
  return 1;
#elif MACOSX
#if __DARWIN_BYTE_ORDER == __DARWIN_BIG_ENDIAN
  assertFalse(isHostLittleEndian());
#elif __DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN
  assert(isHostLittleEndian());
#else
#error Undefined endian type
#endif
#elif WINDOWS
  return 1;
#endif
  return 0;
}

static int _testFlipShortEndian(void) {
  unsigned short s = 0xabcd;
  unsigned short r = flipShortEndian(s);
  assertUnsignedLongEquals(r, 0xcdabul);
  assertIntEquals(s, flipShortEndian(r));
  return 0;
}

static int _testConvertBigEndianShortToPlatform(void) {
  unsigned short s = 0xabcd;
  unsigned short r = convertBigEndianShortToPlatform(s);
#if LINUX
  return 1;
#elif MACOSX
#if __DARWIN_BYTE_ORDER == __DARWIN_BIG_ENDIAN
  assertUnsignedLongEquals(s, (unsigned long)r);
#elif __DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN
  assertUnsignedLongEquals(s, (unsigned long)flipShortEndian(r));
#else
#error Undefined endian type
#endif
#elif WINDOWS
  return 1;
#endif
  return 0;
}

static int _testConvertBigEndianIntToPlatform(void) {
  unsigned int i = 0xdeadbeef;
  unsigned int r = convertBigEndianIntToPlatform(i);
#if LINUX
  return 1;
#elif MACOSX
#if __DARWIN_BYTE_ORDER == __DARWIN_BIG_ENDIAN
  assertUnsignedLongEquals(r, (unsigned long)i);
#elif __DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN
  assertUnsignedLongEquals(r, 0xefbeaddeul);
#else
#error Undefined endian type
#endif
#elif WINDOWS
  return 1;
#endif
  return 0;
}

static int _testConvertLittleEndianIntToPlatform(void) {
  unsigned int i = 0xdeadbeef;
  unsigned int r = convertLittleEndianIntToPlatform(i);
#if LINUX
  return 1;
#elif MACOSX
#if __DARWIN_BYTE_ORDER == __DARWIN_BIG_ENDIAN
  assertUnsignedLongEquals(r, 0xefbeaddeul);
#elif __DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN
  assertUnsignedLongEquals(r, (unsigned long)i);
#else
#error Undefined endian type
#endif
#elif WINDOWS
  return 1;
#endif
  return 0;
}

static int _testConvertBigEndianFloatToPlatform(void) {
  float f = (float)0xdeadbeef;
  float r = convertBigEndianFloatToPlatform(f);
#if LINUX
  return 1;
#elif MACOSX
#if __DARWIN_BYTE_ORDER == __DARWIN_BIG_ENDIAN
  assertDoubleEquals(f, r);
#elif __DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN
  assertDoubleEquals(r, (double)0xefbeadde, TEST_FLOAT_TOLERANCE);
#else
#error Undefined endian type
#endif
#elif WINDOWS
  return 1;
#endif
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
  addTest(testSuite, "ConvertBigEndianShortToPlatform", _testConvertBigEndianShortToPlatform);
  addTest(testSuite, "ConvertBigEndianIntToPlatform", _testConvertBigEndianIntToPlatform);
  addTest(testSuite, "ConvertLittleEndianIntToPlatform", _testConvertLittleEndianIntToPlatform);
  addTest(testSuite, "ConvertBigEndianFloatToPlatform", NULL); //_testConvertBigEndianFloatToPlatform);

  addTest(testSuite, "ConvertByteArrayToUnsignedShort", NULL); //_testConvertByteArrayToUnsignedShort);
  addTest(testSuite, "ConvertByteArrayToUnsignedInt", NULL); //_testConvertByteArrayToUnsignedInt);
  return testSuite;
}
