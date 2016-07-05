//
// PlatformInfoTest.c - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include "base/PlatformInfo.h"

#include "unit/TestRunner.h"

static int _testGetPlatformType(void) {
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

static int _testGetPlatformName(void) {
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

static int _testGetShortPlatformName(void) {
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

static int _testIsHostLittleEndian(void) {
#if HOST_BIG_ENDIAN
  assertFalse(isHostLittleEndian());
#elif HOST_LITTLE_ENDIAN
  assert(isHostLittleEndian());
#endif
  return 0;
}

TestSuite addPlatformInfoTests(void);
TestSuite addPlatformInfoTests(void) {
  TestSuite testSuite = newTestSuite("PlatformInfo", NULL, NULL);

  addTest(testSuite, "GetPlatformType", _testGetPlatformType);
  addTest(testSuite, "GetPlatformName", _testGetPlatformName);
  addTest(testSuite, "GetShortPlatformName", _testGetShortPlatformName);

  addTest(testSuite, "IsHostLittleEndian", _testIsHostLittleEndian);

  return testSuite;
}
