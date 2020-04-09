//
// IntegrationTests.c - MrsWatson
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

#include "base/CharString.h"
#include "base/File.h"
#include "base/PlatformInfo.h"
#include "unit/ApplicationRunner.h"
#include "unit/TestRunner.h"

// Helper functions and macros

#if USE_AUDIOFILE
#define REQUIRES_AUDIOFILE(x) x
#else
#define REQUIRES_AUDIOFILE(x) NULL
#endif

#if USE_FLAC
#define REQUIRES_FLAC(x) x
#else
#define REQUIRES_FLAC(x) NULL
#endif

// These tests are currently broken :(
#define TEST_AIFF_BIT_DEPTHS 0

// The tests for the silence plugin work, but they fail the analysis check for
// silence (obviously). These tests will remain disabled until there is a
// smarter way to specify which analysis functions should be run for each
// integration test.
#define TEST_SILENCE_PLUGIN 0

static CharString getDefaultInputPath(const CharString resourcesPath) {
  return getTestResourcePath(resourcesPath, "audio", "a440-16bit-stereo.pcm");
}

// Tests

static int _testListPlugins(const char *testName,
                            const CharString applicationPath,
                            const CharString resourcesPath) {
  return runIntegrationTest(
      testName, newCharStringWithCString("--list-plugins"), RETURN_CODE_NOT_RUN,
      kTestOutputNone, applicationPath, resourcesPath);
}

static int _testListFileTypes(const char *testName,
                              const CharString applicationPath,
                              const CharString resourcesPath) {
  return runIntegrationTest(
      testName, newCharStringWithCString("--list-file-types"),
      RETURN_CODE_NOT_RUN, kTestOutputNone, applicationPath, resourcesPath);
}

static int _testInvalidArgument(const char *testName,
                                const CharString applicationPath,
                                const CharString resourcesPath) {
  return runIntegrationTest(testName, newCharStringWithCString("--invalid"),
                            RETURN_CODE_INVALID_ARGUMENT, kTestOutputNone,
                            applicationPath, resourcesPath);
}

// Private functions which are needed for the absolute path test
extern const char *_getPlatformVstDirName(const PlatformInfo platform);
extern const char *_getVst2xPlatformExtension();

static int _testLoadPluginWithAbsolutePath(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  File resourcesFile = newFileWithPath(resourcesPath);
  CharString vstDirName = newCharStringWithCString("vst");
  File vstFile = newFileWithParent(resourcesFile, vstDirName);
  freeFile(resourcesFile);
  freeCharString(vstDirName);

  PlatformInfo platformInfo = newPlatformInfo();
  CharString platformDirName =
      newCharStringWithCString(_getPlatformVstDirName(platformInfo));
  File vstPlatformFile = newFileWithParent(vstFile, platformDirName);
  freeCharString(platformDirName);
  freePlatformInfo(platformInfo);
  freeFile(vstFile);

  CharString vstPluginName = newCharStringWithCString("again");
  charStringAppendCString(vstPluginName, _getVst2xPlatformExtension());
  File pluginFile = newFileWithParent(vstPlatformFile, vstPluginName);
  freeCharString(vstPluginName);
  freeFile(vstPlatformFile);

  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin \"%s\" --input \"%s\"",
                              pluginFile->absolutePath->data, inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(inputPath);
  freeFile(pluginFile);
  return result;
}

static int _testRunWithNoPlugins(const char *testName,
                                 const CharString applicationPath,
                                 const CharString resourcesPath) {
  return runIntegrationTest(testName, newCharString(),
                            RETURN_CODE_INVALID_PLUGIN_CHAIN, kTestOutputNone,
                            applicationPath, resourcesPath);
}

static int _testEffectWithNoInputSource(const char *testName,
                                        const CharString applicationPath,
                                        const CharString resourcesPath) {
  return runIntegrationTest(testName,
                            newCharStringWithCString("--plugin again"),
                            RETURN_CODE_MISSING_REQUIRED_OPTION, kTestOutputPcm,
                            applicationPath, resourcesPath);
}

static int _testInstrumentWithNoMidiSource(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  return runIntegrationTest(testName,
                            newCharStringWithCString("--plugin vstxsynth"),
                            RETURN_CODE_MISSING_REQUIRED_OPTION, kTestOutputPcm,
                            applicationPath, resourcesPath);
}

static int
_testPluginChainWithInstrumentNotAtHead(const char *testName,
                                        const CharString applicationPath,
                                        const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin \"again;vstxsynth\" --input \"%s\"",
                              inputPath->data),
      RETURN_CODE_INVALID_PLUGIN_CHAIN, kTestOutputNone, applicationPath,
      resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testPluginWithInvalidPreset(const char *testName,
                                        const CharString applicationPath,
                                        const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin \"again,invalid.fxp\" --input \"%s\"",
                              inputPath->data),
      RETURN_CODE_INVALID_ARGUMENT, kTestOutputNone, applicationPath,
      resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testPresetForWrongPlugin(const char *testName,
                                     const CharString applicationPath,
                                     const CharString resourcesPath) {
  CharString midiPath =
      getTestResourcePath(resourcesPath, "midi", "c-scale.mid");
  CharString presetPath =
      getTestResourcePath(resourcesPath, "presets", "again-test.fxp");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin \"vstxsynth,%s\" --midi-file \"%s\"",
                              presetPath->data, midiPath->data),
      RETURN_CODE_INVALID_ARGUMENT, kTestOutputNone, applicationPath,
      resourcesPath);
  freeCharString(midiPath);
  freeCharString(presetPath);
  return result;
}

static int _testSetInvalidParameter(const char *testName,
                                    const CharString applicationPath,
                                    const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\" --parameter 1,0.5",
                              inputPath->data),
      RETURN_CODE_INVALID_ARGUMENT, kTestOutputNone, applicationPath,
      resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testSetInvalidTimeSignature(const char *testName,
                                        const CharString applicationPath,
                                        const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName, buildTestArgumentString(
                    "--plugin again --input \"%s\" --time-signature invalid",
                    inputPath->data),
      RETURN_CODE_INVALID_ARGUMENT, kTestOutputNone, applicationPath,
      resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testSetInvalidTempo(const char *testName,
                                const CharString applicationPath,
                                const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName, buildTestArgumentString(
                    "--plugin again --input \"%s\" --tempo 0", inputPath->data),
      RETURN_CODE_INVALID_ARGUMENT, kTestOutputNone, applicationPath,
      resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testSetInvalidBlocksize(const char *testName,
                                    const CharString applicationPath,
                                    const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\" --blocksize 0",
                              inputPath->data),
      RETURN_CODE_INVALID_ARGUMENT, kTestOutputNone, applicationPath,
      resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testSetInvalidBitDepth(const char *testName,
                                   const CharString applicationPath,
                                   const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\" --bit-depth 5",
                              inputPath->data),
      RETURN_CODE_INVALID_ARGUMENT, kTestOutputNone, applicationPath,
      resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testSetInvalidChannelCount(const char *testName,
                                       const CharString applicationPath,
                                       const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\" --channels 0",
                              inputPath->data),
      RETURN_CODE_INVALID_ARGUMENT, kTestOutputNone, applicationPath,
      resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testSetInvalidSampleRate(const char *testName,
                                     const CharString applicationPath,
                                     const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\" --sample-rate 0",
                              inputPath->data),
      RETURN_CODE_INVALID_ARGUMENT, kTestOutputNone, applicationPath,
      resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessPcmFile16BitMono(const char *testName,
                                        const CharString applicationPath,
                                        const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-16bit-mono.pcm");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\" --channels 1",
                              inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessPcmFile16BitStereo(const char *testName,
                                          const CharString applicationPath,
                                          const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-16bit-stereo.pcm");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessWaveFile16BitMono(const char *testName,
                                         const CharString applicationPath,
                                         const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-16bit-mono.wav");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputWave, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessWaveFile16BitStereo(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-16bit-stereo.wav");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputWave, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessWaveFile8BitMono(const char *testName,
                                        const CharString applicationPath,
                                        const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-8bit-mono.wav");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputWave, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessWaveFile8BitStereo(const char *testName,
                                          const CharString applicationPath,
                                          const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-8bit-stereo.wav");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputWave, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessWaveFile24BitMono(const char *testName,
                                         const CharString applicationPath,
                                         const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-24bit-mono.wav");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputWave, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessWaveFile24BitStereo(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-24bit-stereo.wav");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputWave, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessWaveFile32BitMono(const char *testName,
                                         const CharString applicationPath,
                                         const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-32bit-mono.wav");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputWave, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessWaveFile32BitStereo(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-32bit-stereo.wav");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputWave, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessWaveFileFfmpeg(const char *testName,
                                      const CharString applicationPath,
                                      const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-stereo-ffmpeg.wav");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputWave, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessAiffFile16BitMono(const char *testName,
                                         const CharString applicationPath,
                                         const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-16bit-mono.aiff");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputAiff, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessAiffFile16BitStereo(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-16bit-stereo.aiff");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputAiff, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

#if TEST_AIFF_BIT_DEPTHS
static int _testProcessAiffFile8BitMono(const char *testName,
                                        const CharString applicationPath,
                                        const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-8bit-mono.aiff");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputAiff, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessAiffFile8BitStereo(const char *testName,
                                          const CharString applicationPath,
                                          const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-8bit-stereo.aiff");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputAiff, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessAiffFile24BitMono(const char *testName,
                                         const CharString applicationPath,
                                         const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-24bit-mono.aiff");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputAiff, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessAiffFile24BitStereo(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-24bit-stereo.aiff");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputAiff, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessAiffFile32BitMono(const char *testName,
                                         const CharString applicationPath,
                                         const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-32bit-mono.aiff");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputAiff, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessAiffFile32BitStereo(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-32bit-stereo.aiff");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputAiff, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}
#endif

#if USE_FLAC
static int _testProcessFlacFile16BitMono(const char *testName,
                                         const CharString applicationPath,
                                         const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-16bit-mono.flac");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputFlac, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessFlacFile16BitStereo(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-16bit-stereo.flac");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputFlac, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessFlacFile8BitMono(const char *testName,
                                        const CharString applicationPath,
                                        const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-8bit-mono.flac");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputFlac, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessFlacFile8BitStereo(const char *testName,
                                          const CharString applicationPath,
                                          const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-8bit-stereo.flac");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputFlac, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessFlacFile24BitMono(const char *testName,
                                         const CharString applicationPath,
                                         const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-24bit-mono.flac");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputFlac, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessFlacFile24BitStereo(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-24bit-stereo.flac");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputFlac, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessFlacFile32BitMono(const char *testName,
                                         const CharString applicationPath,
                                         const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-32bit-mono.flac");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputFlac, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessFlacFile32BitStereo(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  CharString inputPath =
      getTestResourcePath(resourcesPath, "audio", "a440-32bit-stereo.flac");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputFlac, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}
#endif

static int _testProcessWithSampleRate(const char *testName,
                                      const CharString applicationPath,
                                      const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString(
          "--plugin again --input \"%s\" --sample-rate 48000", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessWithBlocksize(const char *testName,
                                     const CharString applicationPath,
                                     const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\" --blocksize 128",
                              inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessWithTimeSignature(const char *testName,
                                         const CharString applicationPath,
                                         const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName, buildTestArgumentString(
                    "--plugin again --input \"%s\" --time-signature 3/4",
                    inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testSetParameter(const char *testName,
                             const CharString applicationPath,
                             const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\" --parameter 0,0.5",
                              inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testInternalLimiter(const char *testName,
                                const CharString applicationPath,
                                const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName, buildTestArgumentString("--plugin mrs_limiter --input \"%s\"",
                                        inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testInternalGainPlugin(const char *testName,
                                   const CharString applicationPath,
                                   const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName, buildTestArgumentString(
                    "--plugin mrs_gain --parameter 0,0.5 --input \"%s\"",
                    inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int
_testInternalGainPluginInvalidParameter(const char *testName,
                                        const CharString applicationPath,
                                        const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName, buildTestArgumentString(
                    "--plugin mrs_gain --parameter 1,0.5 --input \"%s\"",
                    inputPath->data),
      RETURN_CODE_INVALID_ARGUMENT, kTestOutputNone, applicationPath,
      resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testInternalPassthruPlugin(const char *testName,
                                       const CharString applicationPath,
                                       const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName, buildTestArgumentString("--plugin mrs_passthru --input \"%s\"",
                                        inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

#if TEST_SILENCE_PLUGIN
static int _testInternalSilenceGenerator(const char *testName,
                                         const CharString applicationPath,
                                         const CharString resourcesPath) {
  return runIntegrationTest(
      testName,
      newCharStringWithCString("--plugin mrs_silence --max-time 1000"),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
}
#endif

static int _testProcessWithAgainPlugin(const char *testName,
                                       const CharString applicationPath,
                                       const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin again --input \"%s\"", inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(inputPath);
  return result;
}

static int _testProcessWithVstxsynthPlugin(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  CharString midiFile =
      getTestResourcePath(resourcesPath, "midi", "c-scale.mid");
  int result = runIntegrationTest(
      testName, buildTestArgumentString("--plugin vstxsynth --midi-file \"%s\"",
                                        midiFile->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(midiFile);
  return result;
}

static int _testProcessMidiType1File(const char *testName,
                                           const CharString applicationPath,
                                           const CharString resourcesPath) {
  CharString midiFile =
      getTestResourcePath(resourcesPath, "midi", "children.mid");
  int result = runIntegrationTest(
      testName, buildTestArgumentString(
          "--plugin vstxsynth --midi-file \"%s\" --midi-track 4", midiFile->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(midiFile);
  return result;
}

static int _testProcessEffectChain(const char *testName,
                                   const CharString applicationPath,
                                   const CharString resourcesPath) {
  CharString midiFile =
      getTestResourcePath(resourcesPath, "midi", "c-scale.mid");
  int result = runIntegrationTest(
      testName,
      buildTestArgumentString("--plugin vstxsynth,again --midi-file \"%s\"",
                              midiFile->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(midiFile);
  return result;
}

static int _testLoadFxpPresetInVst(const char *testName,
                                   const CharString applicationPath,
                                   const CharString resourcesPath) {
  CharString inputPath = getDefaultInputPath(resourcesPath);
  CharString presetPath =
      getTestResourcePath(resourcesPath, "presets", "again-test.fxp");
  int result = runIntegrationTest(
      testName, buildTestArgumentString("--plugin \"again,%s\" --input \"%s\"",
                                        presetPath->data, inputPath->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(inputPath);
  freeCharString(presetPath);
  return result;
}

static int _testLoadInternalProgramInVst(const char *testName,
                                         const CharString applicationPath,
                                         const CharString resourcesPath) {
  CharString midiFile =
      getTestResourcePath(resourcesPath, "midi", "c-scale.mid");
  int result = runIntegrationTest(
      testName, buildTestArgumentString(
                    "--plugin vstxsynth,2 --midi-file \"%s\"", midiFile->data),
      RETURN_CODE_SUCCESS, kTestOutputPcm, applicationPath, resourcesPath);
  freeCharString(midiFile);
  return result;
}

TestSuite addIntegrationTests(File mrsWatsonExePath, File resourcesPath);

TestSuite addIntegrationTests(File mrsWatsonExePath, File resourcesPath) {
  TestSuite testSuite = newTestSuite("Integration", NULL, NULL);
  testSuite->applicationPath =
      newCharStringWithCString(mrsWatsonExePath->absolutePath->data);
  testSuite->resourcesPath =
      newCharStringWithCString(resourcesPath->absolutePath->data);

  // Basic non-processing operations
  addTestWithPaths(testSuite, "List plugins", _testListPlugins);
  addTestWithPaths(testSuite, "List file types", _testListFileTypes);
  addTestWithPaths(testSuite, "Invalid arugment", _testInvalidArgument);

  // Plugin loading
  addTestWithPaths(testSuite, "Load plugin with absolute path",
                   _testLoadPluginWithAbsolutePath);

  // Invalid configurations
  addTestWithPaths(testSuite, "Run with no plugins", _testRunWithNoPlugins);
  addTestWithPaths(testSuite, "Effect with no input source",
                   _testEffectWithNoInputSource);
  addTestWithPaths(testSuite, "Instrument with no MIDI source",
                   _testInstrumentWithNoMidiSource);
  addTestWithPaths(testSuite, "Plugin chain with instrument not at head",
                   _testPluginChainWithInstrumentNotAtHead);
  addTestWithPaths(testSuite, "Plugin with invalid preset",
                   _testPluginWithInvalidPreset);
  addTestWithPaths(testSuite, "Preset for wrong plugin",
                   _testPresetForWrongPlugin);
  addTestWithPaths(testSuite, "Set invalid parameter",
                   _testSetInvalidParameter);
  addTestWithPaths(testSuite, "Set invalid time signature",
                   _testSetInvalidTimeSignature);
  addTestWithPaths(testSuite, "Set invalid tempo", _testSetInvalidTempo);
  addTestWithPaths(testSuite, "Set invalid blocksize",
                   _testSetInvalidBlocksize);
  addTestWithPaths(testSuite, "Set invalid bit depth", _testSetInvalidBitDepth);
  addTestWithPaths(testSuite, "Set invalid channel count",
                   _testSetInvalidChannelCount);
  addTestWithPaths(testSuite, "Set invalid sample rate",
                   _testSetInvalidSampleRate);

  // PCM files
  // TODO: Should add tests for other bit depths
  addTestWithPaths(testSuite, "Process 16-bit PCM file (mono)",
                   _testProcessPcmFile16BitMono);
  addTestWithPaths(testSuite, "Process 16-bit PCM file (stereo)",
                   _testProcessPcmFile16BitStereo);

  // WAVE files
  addTestWithPaths(testSuite, "Process 16-bit WAVE file (mono)",
                   _testProcessWaveFile16BitMono);
  addTestWithPaths(testSuite, "Process 16-bit WAVE file (stereo)",
                   _testProcessWaveFile16BitStereo);
  addTestWithPaths(testSuite, "Process 8-bit WAVE file (mono)",
                   REQUIRES_AUDIOFILE(_testProcessWaveFile8BitMono));
  addTestWithPaths(testSuite, "Process 8-bit WAVE file (stereo)",
                   REQUIRES_AUDIOFILE(_testProcessWaveFile8BitStereo));
  addTestWithPaths(testSuite, "Process 24-bit WAVE file (mono)",
                   REQUIRES_AUDIOFILE(_testProcessWaveFile24BitMono));
  addTestWithPaths(testSuite, "Process 24-bit WAVE file (stereo)",
                   REQUIRES_AUDIOFILE(_testProcessWaveFile24BitStereo));
  addTestWithPaths(testSuite, "Process 32-bit WAVE file (mono)",
                   REQUIRES_AUDIOFILE(_testProcessWaveFile32BitMono));
  addTestWithPaths(testSuite, "Process 32-bit WAVE file (stereo)",
                   REQUIRES_AUDIOFILE(_testProcessWaveFile32BitStereo));
  addTestWithPaths(testSuite, "Process FFMpeg WAVE file (stereo)",
                   _testProcessWaveFileFfmpeg);

  // AIFF files
  addTestWithPaths(testSuite, "Process 16-bit AIFF file (mono)",
                   REQUIRES_AUDIOFILE(_testProcessAiffFile16BitMono));
  addTestWithPaths(testSuite, "Process 16-bit AIFF file (stereo)",
                   REQUIRES_AUDIOFILE(_testProcessAiffFile16BitStereo));
#if TEST_AIFF_BIT_DEPTHS
  // 8-bit AIFF will require some extra work in SampleSourceAudiofile
  addTestWithPaths(testSuite, "Process 8-bit AIFF file (mono)",
                   REQUIRES_AUDIOFILE(_testProcessAiffFile8BitMono));
  addTestWithPaths(testSuite, "Process 8-bit AIFF file (stereo)",
                   REQUIRES_AUDIOFILE(_testProcessAiffFile8BitStereo));
  addTestWithPaths(testSuite, "Process 24-bit AIFF file (mono)",
                   REQUIRES_AUDIOFILE(_testProcessAiffFile24BitMono));
  addTestWithPaths(testSuite, "Process 24-bit AIFF file (stereo)",
                   REQUIRES_AUDIOFILE(_testProcessAiffFile24BitStereo));
  // 32-bit AIFF will require some extra work in SampleSourceAudiofile
  addTestWithPaths(testSuite, "Process 32-bit AIFF file (mono)",
                   REQUIRES_AUDIOFILE(_testProcessAiffFile32BitMono));
  addTestWithPaths(testSuite, "Process 32-bit AIFF file (stereo)",
                   REQUIRES_AUDIOFILE(_testProcessAiffFile32BitStereo));
#endif

  // FLAC files
  addTestWithPaths(testSuite, "Process 8-bit FLAC file (mono)",
                   REQUIRES_FLAC(_testProcessFlacFile8BitMono));
  addTestWithPaths(testSuite, "Process 8-bit FLAC file (stereo)",
                   REQUIRES_FLAC(_testProcessFlacFile8BitStereo));
  addTestWithPaths(testSuite, "Process 16-bit FLAC file (mono)",
                   REQUIRES_FLAC(_testProcessFlacFile16BitMono));
  addTestWithPaths(testSuite, "Process 16-bit FLAC file (stereo)",
                   REQUIRES_FLAC(_testProcessFlacFile16BitStereo));
  addTestWithPaths(testSuite, "Process 24-bit FLAC file (mono)",
                   REQUIRES_FLAC(_testProcessFlacFile24BitMono));
  addTestWithPaths(testSuite, "Process 24-bit FLAC file (stereo)",
                   REQUIRES_FLAC(_testProcessFlacFile24BitStereo));
  addTestWithPaths(testSuite, "Process 32-bit FLAC file (mono)",
                   REQUIRES_FLAC(_testProcessFlacFile32BitMono));
  addTestWithPaths(testSuite, "Process 32-bit FLAC file (stereo)",
                   REQUIRES_FLAC(_testProcessFlacFile32BitStereo));

  // Audio settings
  addTestWithPaths(testSuite, "Process with sample rate",
                   _testProcessWithSampleRate);
  addTestWithPaths(testSuite, "Process with blocksize",
                   _testProcessWithBlocksize);
  addTestWithPaths(testSuite, "Process with time signature",
                   _testProcessWithTimeSignature);

  // Parameter tests
  addTestWithPaths(testSuite, "Set parameter", _testSetParameter);

  // Internal plugins
  addTestWithPaths(testSuite, "Internal limiter", _testInternalLimiter);
  addTestWithPaths(testSuite, "Internal gain plugin", _testInternalGainPlugin);
  addTestWithPaths(testSuite, "Internal gain plugin invalid parameter",
                   _testInternalGainPluginInvalidParameter);
  addTestWithPaths(testSuite, "Internal passthru plugin",
                   _testInternalPassthruPlugin);

#if TEST_SILENCE_PLUGIN
  addTestWithPaths(testSuite, "Internal silence generator",
                   _testInternalSilenceGenerator);
#endif

  // Plugin processing tests
  addTestWithPaths(testSuite, "Process audio with again plugin",
                   _testProcessWithAgainPlugin);
  addTestWithPaths(testSuite, "Process MIDI with vstxsynth plugin",
                   _testProcessWithVstxsynthPlugin);
  addTestWithPaths(testSuite, "Process MIDI type 1 file",
                   _testProcessMidiType1File);
  addTestWithPaths(testSuite, "Process effect chain", _testProcessEffectChain);
  addTestWithPaths(testSuite, "Load FXP preset in VST",
                   _testLoadFxpPresetInVst);
  addTestWithPaths(testSuite, "Load internal program in VST",
                   _testLoadInternalProgramInVst);

  return testSuite;
}
