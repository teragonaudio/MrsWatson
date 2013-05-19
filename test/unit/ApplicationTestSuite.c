#include "ApplicationRunner.h"

int runApplicationTestSuite(TestEnvironment environment);
int runApplicationTestSuite(TestEnvironment environment) {
  // Test resource paths
  const char* resourcesPath = environment->resourcesPath;
  const char* a440_mono_pcm = getTestResourceFilename(resourcesPath, "audio", "a440-mono.pcm");
  const char* a440_stereo_pcm = getTestResourceFilename(resourcesPath, "audio", "a440-stereo.pcm");
  const char* a440_stereo_wav = getTestResourceFilename(resourcesPath, "audio", "a440-stereo.wav");
  const char* c_scale_mid = getTestResourceFilename(resourcesPath, "midi", "c-scale.mid");
  const char* again_test_fxp = getTestResourceFilename(resourcesPath, "presets", "again-test.fxp");

  // Basic non-processing operations
  runApplicationTest(environment, "List plugins",
    newCharStringWithCString("--list-plugins"),
    RETURN_CODE_NOT_RUN, NULL
  );
  runApplicationTest(environment, "List file types",
    newCharStringWithCString("--list-file-types"),
    RETURN_CODE_NOT_RUN, NULL
  );
  runApplicationTest(environment, "Invalid argument",
    newCharStringWithCString("--invalid"),
    RETURN_CODE_INVALID_ARGUMENT, NULL
  );

  // Invalid configurations
  runApplicationTest(environment, "Run with no plugins",
    newCharString(),
    RETURN_CODE_INVALID_PLUGIN_CHAIN, NULL
  );
  runApplicationTest(environment, "Effect with no input source",
    newCharStringWithCString("--plugin again"),
    RETURN_CODE_MISSING_REQUIRED_OPTION, NULL
  );
  runApplicationTest(environment, "Instrument with no MIDI source",
    newCharStringWithCString("--plugin vstxsynth"),
    RETURN_CODE_MISSING_REQUIRED_OPTION, NULL
  );
  runApplicationTest(environment, "Plugin chain with instrument not at head",
    buildTestArgumentString("--plugin \"again;vstxsynth\" --input \"%s\"", a440_stereo_pcm),
    RETURN_CODE_INVALID_PLUGIN_CHAIN, NULL
  );
  runApplicationTest(environment, "Plugin with invalid preset",
    buildTestArgumentString("--plugin \"again,invalid.fxp\" --input \"%s\"", a440_stereo_pcm),
    RETURN_CODE_INVALID_ARGUMENT, NULL
  );
  runApplicationTest(environment, "Preset for wrong plugin",
    buildTestArgumentString("--plugin \"vstxsynth,%s\" --midi-file \"%s\"", again_test_fxp, c_scale_mid),
    RETURN_CODE_INVALID_ARGUMENT, NULL
  );

  // Audio file types
  runApplicationTest(environment, "Read WAV file",
    buildTestArgumentString("--plugin again --input \"%s\"", a440_stereo_wav),
    RETURN_CODE_SUCCESS, kDefaultTestOutputFileType
  );
  runApplicationTest(environment, "Write WAV file",
    buildTestArgumentString("--plugin again --input \"%s\"", a440_stereo_pcm),
    RETURN_CODE_SUCCESS, "wav"
  );

  // Configuration tests
  runApplicationTest(environment, "Read mono input source",
    buildTestArgumentString("--plugin again --input \"%s\" --channels 1", a440_mono_pcm),
    RETURN_CODE_SUCCESS, kDefaultTestOutputFileType
  );
  runApplicationTest(environment, "Read with user-defined sample rate",
    buildTestArgumentString("--plugin again --input \"%s\" --sample-rate 48000", a440_stereo_pcm),
    RETURN_CODE_SUCCESS, kDefaultTestOutputFileType
  );
  runApplicationTest(environment, "Read with user-defined blocksize",
    buildTestArgumentString("--plugin again --input \"%s\" --blocksize 128", a440_stereo_pcm),
    RETURN_CODE_SUCCESS, kDefaultTestOutputFileType
  );
  runApplicationTest(environment, "Add tail-time",
    buildTestArgumentString("--plugin again --input \"%s\" --tail-time 10", a440_stereo_pcm),
    RETURN_CODE_SUCCESS, kDefaultTestOutputFileType
  );

  // Internal plugins
  runApplicationTest(environment, "Process with internal passthru plugin",
    buildTestArgumentString("--plugin mrs_passthru --input \"%s\"", a440_stereo_pcm),
    RETURN_CODE_SUCCESS, kDefaultTestOutputFileType
  );

  // Plugin processing tests
  runApplicationTest(environment, "Process audio with again plugin",
    buildTestArgumentString("--plugin again --input \"%s\"", a440_stereo_pcm),
    RETURN_CODE_SUCCESS, kDefaultTestOutputFileType
  );
  runApplicationTest(environment, "Process MIDI with vstxsynth plugin",
    buildTestArgumentString("--plugin vstxsynth --midi-file \"%s\"", c_scale_mid),
    RETURN_CODE_SUCCESS, kDefaultTestOutputFileType
  );
  runApplicationTest(environment, "Process effect chain",
    buildTestArgumentString("--plugin vstxsynth,again --midi-file \"%s\"", c_scale_mid),
    RETURN_CODE_SUCCESS, kDefaultTestOutputFileType
  );
  runApplicationTest(environment, "Load FXP preset to VST",
    buildTestArgumentString("--plugin \"again,%s\" --input \"%s\"", again_test_fxp, a440_stereo_pcm),
    RETURN_CODE_SUCCESS, kDefaultTestOutputFileType
  );

  fprintf(stderr, "\n== Ran %d application tests: %d passed, %d failed, %d skipped ==\n",
    environment->results->numSuccess +
    environment->results->numFail +
    environment->results->numSkips,
    environment->results->numSuccess,
    environment->results->numFail,
    environment->results->numSkips);

  return environment->results->numFail;
}
