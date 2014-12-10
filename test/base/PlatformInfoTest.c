#include "unit/TestRunner.h"
#include "base/PlatformInfo.h"

static int _testGetPlatformType(void)
{
    PlatformInfo platform = newPlatformInfo();
#if LINUX
    assertIntEquals(PLATFORM_LINUX, platform->type);
#elif MACOSX
    assertIntEquals(PLATFORM_MACOSX, platform->type);
#elif WINDOWS
    assertIntEquals(PLATFORM_WINDOWS, platform->type);
#else
    assertIntEquals(PLATFORM_UNSUPPORTED, platform->type);
#endif
    freePlatformInfo(platform);
    return 0;
}

static int _testGetPlatformName(void)
{
    PlatformInfo platform = newPlatformInfo();
#if LINUX
    assertCharStringContains("Linux", platform->name);
#elif MACOSX
    assertCharStringContains("Mac OS X", platform->name);
#elif WINDOWS
    assertCharStringContains("Windows", platform->name);
#else
    assertCharStringEquals("Unsupported platform", platform->name);
#endif
    freePlatformInfo(platform);
    return 0;
}

static int _testGetShortPlatformName(void)
{
    PlatformInfo platform = newPlatformInfo();
#if LINUX

    if (platformInfoIsHost64Bit() && platformInfoIsRuntime64Bit()) {
        assertCharStringEquals("Linux-x86_64", platform->shortName);
    } else {
        assertCharStringEquals("Linux-i686", platform->shortName);
    }

#elif MACOSX
    assertCharStringEquals("Mac OS X", platform->shortName);
#elif WINDOWS

    if (platformInfoIsHost64Bit() && platformInfoIsRuntime64Bit()) {
        assertCharStringEquals("Windows 64-bit", platform->shortName);
    } else {
        assertCharStringEquals("Windows 32-bit", platform->shortName);
    }

#else
    assertCharStringEquals("Unsupported", platform->shortName);
#endif
    freePlatformInfo(platform);
    return 0;
}

static int _testIsHostLittleEndian(void)
{
#if HOST_BIG_ENDIAN
    assertFalse(isHostLittleEndian());
#elif HOST_LITTLE_ENDIAN
    assert(isHostLittleEndian());
#endif
    return 0;
}

TestSuite addPlatformInfoTests(void);
TestSuite addPlatformInfoTests(void)
{
    TestSuite testSuite = newTestSuite("PlatformInfo", NULL, NULL);

    addTest(testSuite, "GetPlatformType", _testGetPlatformType);
    addTest(testSuite, "GetPlatformName", _testGetPlatformName);
    addTest(testSuite, "GetShortPlatformName", _testGetShortPlatformName);

    addTest(testSuite, "IsHostLittleEndian", _testIsHostLittleEndian);

    return testSuite;
}