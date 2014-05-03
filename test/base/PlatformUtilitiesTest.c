#include "unit/TestRunner.h"
#include "time/TaskTimer.h"

#if LINUX
#if defined(__BYTE_ORDER__)
#define HOST_BIG_ENDIAN (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define HOST_LITTLE_ENDIAN (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#endif
#elif MACOSX
#include <machine/endian.h>
#define HOST_BIG_ENDIAN (__DARWIN_BYTE_ORDER == __DARWIN_BIG_ENDIAN)
#define HOST_LITTLE_ENDIAN (__DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN)
#elif WINDOWS
// Windows is always little-endian, see: http://support.microsoft.com/kb/102025
#define HOST_BIG_ENDIAN 0
#define HOST_LITTLE_ENDIAN 1
#endif

#if !defined(HOST_BIG_ENDIAN) || !defined(HOST_LITTLE_ENDIAN)
#error Host platform endian-ness not known
#error Please define either HOST_BIG_ENDIAN or HOST_LITTLE_ENDIAN
#elif HOST_BIG_ENDIAN && HOST_LITTLE_ENDIAN
#error Both HOST_BIG_ENDIAN and HOST_LITTLE_ENDIAN cannot be defined to 1!
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
#if HOST_BIG_ENDIAN
  assertFalse(isHostLittleEndian());
#elif HOST_LITTLE_ENDIAN
  assert(isHostLittleEndian());
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
#if HOST_BIG_ENDIAN
  assertUnsignedLongEquals(s, (unsigned long)r);
#elif HOST_LITTLE_ENDIAN
  assertUnsignedLongEquals(s, (unsigned long)flipShortEndian(r));
#endif
  return 0;
}

static int _testConvertBigEndianIntToPlatform(void) {
  unsigned int i = 0xdeadbeef;
  unsigned int r = convertBigEndianIntToPlatform(i);
#if HOST_BIG_ENDIAN
  assertUnsignedLongEquals(r, (unsigned long)i);
#elif HOST_LITTLE_ENDIAN
  assertUnsignedLongEquals(r, 0xefbeaddeul);
#endif
  return 0;
}

static int _testConvertLittleEndianIntToPlatform(void) {
  unsigned int i = 0xdeadbeef;
  unsigned int r = convertLittleEndianIntToPlatform(i);
#if HOST_BIG_ENDIAN
  assertUnsignedLongEquals(r, 0xefbeaddeul);
#elif HOST_LITTLE_ENDIAN
  assertUnsignedLongEquals(r, (unsigned long)i);
#endif
  return 0;
}

static int _testConvertBigEndianFloatToPlatform(void) {
  // Generate an integer with a known value and convert it to a float using pointer trickery
  int i = 0xdeadbeef;
  int i2 = 0xefbeadbe;
  float *f = (float*)&i;
  float *f2 = (float*)&i2;
  float r = convertBigEndianFloatToPlatform(*f);
  // Unfortunately, the above pointer trickery will result in a really huge number, so the
  // standard comparison tolerance is *way* too low. This number is the lowest possible
  // result that we should accept for this test.
  const double bigFloatTolerance = 3.02231e+24;

#if HOST_BIG_ENDIAN
  assertDoubleEquals(*f, r);
#elif HOST_LITTLE_ENDIAN
  // Sanity check to make sure that the actual result is the really huge number which we
  // are expecting.
  assert(fabs(r) > bigFloatTolerance);
  assertDoubleEquals(r, *f2, bigFloatTolerance);
#endif
  return 0;
}

static int _testConvertByteArrayToUnsignedShort(void) {
  byte* b = (byte*)malloc(sizeof(byte) * 2);
  int i;
  unsigned short s;
  for(i = 0; i < 2; i++) {
    b[i] = (byte)(0xaa + i);
  }
  s = convertByteArrayToUnsignedShort(b);

#if HOST_BIG_ENDIAN
  assertUnsignedLongEquals(s, 0xaaabul);
#elif HOST_LITTLE_ENDIAN
  assertUnsignedLongEquals(s, 0xabaaul);
#endif
  return 0;
}

static int _testConvertByteArrayToUnsignedInt(void) {
  byte* b = (byte*)malloc(sizeof(byte) * 2);
  int i;
  unsigned int s;
  for(i = 0; i < 4; i++) {
    b[i] = (byte)(0xaa + i);
  }
  s = convertByteArrayToUnsignedInt(b);

#if HOST_BIG_ENDIAN
  assertUnsignedLongEquals(s, 0xaaabacadul);
#elif HOST_LITTLE_ENDIAN
  assertUnsignedLongEquals(s, 0xadacabaaul);
#endif
  return 0;
}

static int _testSleepMilliseconds(void) {
  double elapsedTime;
  TaskTimer t = newTaskTimerWithCString("test", "test");
  taskTimerStart(t);
  sleepMilliseconds(12);
  elapsedTime = taskTimerStop(t);
  assertTimeEquals(elapsedTime, 12, 0.1);
  freeTaskTimer(t);
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
  addTest(testSuite, "ConvertBigEndianFloatToPlatform", _testConvertBigEndianFloatToPlatform);

  addTest(testSuite, "ConvertByteArrayToUnsignedShort", _testConvertByteArrayToUnsignedShort);
  addTest(testSuite, "ConvertByteArrayToUnsignedInt", _testConvertByteArrayToUnsignedInt);

  addTest(testSuite, "SleepMilliseconds", _testSleepMilliseconds);
  return testSuite;
}
