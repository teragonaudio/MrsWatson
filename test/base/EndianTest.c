#include "unit/TestRunner.h"
#include "base/Endian.h"

#include "unit/TestRunner.h"

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

static int _testFlipShortEndian(void)
{
    unsigned short s = 0xabcd;
    unsigned short r = flipShortEndian(s);
    assertUnsignedLongEquals(0xcdabul, r);
    assertIntEquals(s, flipShortEndian(r));
    return 0;
}

static int _testConvertBigEndianShortToPlatform(void)
{
    unsigned short s = 0xabcd;
    unsigned short r = convertBigEndianShortToPlatform(s);
#if HOST_BIG_ENDIAN
    assertUnsignedLongEquals(s, (unsigned long)r);
#elif HOST_LITTLE_ENDIAN
    assertUnsignedLongEquals((unsigned long)s, (unsigned long)flipShortEndian(r));
#endif
    return 0;
}

static int _testConvertBigEndianIntToPlatform(void)
{
    unsigned int i = 0xdeadbeef;
    unsigned int r = convertBigEndianIntToPlatform(i);
#if HOST_BIG_ENDIAN
    assertUnsignedLongEquals((unsigned long)i, r);
#elif HOST_LITTLE_ENDIAN
    assertUnsignedLongEquals(0xefbeaddeul, r);
#endif
    return 0;
}

static int _testConvertLittleEndianIntToPlatform(void)
{
    unsigned int i = 0xdeadbeef;
    unsigned int r = convertLittleEndianIntToPlatform(i);
#if HOST_BIG_ENDIAN
    assertUnsignedLongEquals(0xefbeaddeul, r);
#elif HOST_LITTLE_ENDIAN
    assertUnsignedLongEquals((unsigned long)i, r);
#endif
    return 0;
}

static int _testConvertBigEndianFloatToPlatform(void)
{
    // Generate an integer with a known value and convert it to a float using pointer trickery
    int i = 0xdeadbeef;
    int i2 = 0xefbeadbe;
    float *f = (float *)&i;
    float *f2 = (float *)&i2;
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
    assertDoubleEquals(*f2, r, bigFloatTolerance);
#endif
    return 0;
}

static int _testConvertByteArrayToUnsignedShort(void)
{
    byte *b = (byte *)malloc(sizeof(byte) * 2);
    int i;
    unsigned short s;

    for (i = 0; i < 2; i++) {
        b[i] = (byte)(0xaa + i);
    }

    s = convertByteArrayToUnsignedShort(b);

#if HOST_BIG_ENDIAN
    assertUnsignedLongEquals(0xaaabul, s);
#elif HOST_LITTLE_ENDIAN
    assertUnsignedLongEquals(0xabaaul, s);
#endif
    return 0;
}

static int _testConvertByteArrayToUnsignedInt(void)
{
    byte *b = (byte *)malloc(sizeof(byte) * 2);
    int i;
    unsigned int s;

    for (i = 0; i < 4; i++) {
        b[i] = (byte)(0xaa + i);
    }

    s = convertByteArrayToUnsignedInt(b);

#if HOST_BIG_ENDIAN
    assertUnsignedLongEquals(0xaaabacadul, s);
#elif HOST_LITTLE_ENDIAN
    assertUnsignedLongEquals(0xadacabaaul, s);
#endif
    return 0;
}

TestSuite addEndianTests(void);
TestSuite addEndianTests(void)
{
    TestSuite testSuite = newTestSuite("Endian", NULL, NULL);

    addTest(testSuite, "FlipShortEndian", _testFlipShortEndian);
    addTest(testSuite, "ConvertBigEndianShortToPlatform", _testConvertBigEndianShortToPlatform);
    addTest(testSuite, "ConvertBigEndianIntToPlatform", _testConvertBigEndianIntToPlatform);
    addTest(testSuite, "ConvertLittleEndianIntToPlatform", _testConvertLittleEndianIntToPlatform);
    addTest(testSuite, "ConvertBigEndianFloatToPlatform", _testConvertBigEndianFloatToPlatform);

    addTest(testSuite, "ConvertByteArrayToUnsignedShort", _testConvertByteArrayToUnsignedShort);
    addTest(testSuite, "ConvertByteArrayToUnsignedInt", _testConvertByteArrayToUnsignedInt);

    return testSuite;
}
