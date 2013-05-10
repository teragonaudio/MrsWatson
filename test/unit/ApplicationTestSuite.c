#include "ApplicationRunner.h"

int runApplicationTestSuite(TestEnvironment environment);
int runApplicationTestSuite(TestEnvironment environment) {
  // Test resource paths
  const char* resourcesPath = environment->resourcesPath;
  const char* a440_stereo_pcm = getTestFilename(resourcesPath, "audio", "a440-stereo.pcm");
  const char* a440_stereo_wav = getTestFilename(resourcesPath, "audio", "a440-stereo.wav");
  const char* c_scale_mid = getTestFilename(resourcesPath, "midi", "c-scale.mid");
  const char* again_test_fxp = getTestFilename(resourcesPath, "presets", "again-test.fxp");

  // Basic non-processing operations
  runApplicationTest(environment, "List plugins",
    newCharStringWithCString("--list-plugins"),
    RETURN_CODE_NOT_RUN, false
  );
  runApplicationTest(environment, "List file types",
    newCharStringWithCString("--list-file-types"),
    RETURN_CODE_NOT_RUN, false
  );
  runApplicationTest(environment, "Invalid argument",
    newCharStringWithCString("--invalid"),
    RETURN_CODE_INVALID_ARGUMENT, false
  );

  // Invalid configurations
  runApplicationTest(environment, "Run with no plugins",
    newCharString(),
    RETURN_CODE_INVALID_PLUGIN_CHAIN, false
  );
  runApplicationTest(environment, "Effect with no input source",
    newCharStringWithCString("--plugin again"),
    RETURN_CODE_MISSING_REQUIRED_OPTION, false
  );
  runApplicationTest(environment, "Instrument with no MIDI source",
    newCharStringWithCString("--plugin vstxsynth"),
    RETURN_CODE_MISSING_REQUIRED_OPTION, false
  );
  runApplicationTest(environment, "Plugin chain with instrument not at head",
    buildTestArgumentString("--plugin \"again;vstxsynth\" --input \"%s\"", a440_stereo_pcm),
    RETURN_CODE_INVALID_PLUGIN_CHAIN, false
  );
  runApplicationTest(environment, "Plugin with invalid preset",
    buildTestArgumentString("--plugin \"again,invalid.fxp\" --input \"%s\"", a440_stereo_pcm),
    RETURN_CODE_INVALID_ARGUMENT, false
  );
  runApplicationTest(environment, "Preset for wrong plugin",
    buildTestArgumentString("--plugin \"vstxsynth,%s\" --midi-file \"%s\"", again_test_fxp, c_scale_mid),
    RETURN_CODE_INVALID_ARGUMENT, false
  );

  // Audio file types
  runApplicationTest(environment, "Read WAV file",
    buildTestArgumentString("--plugin again --input \"%s\"", a440_stereo_wav),
    RETURN_CODE_SUCCESS, true
  );

  // Processing tests
  runApplicationTest(environment, "Process audio with again plugin",
    buildTestArgumentString("--plugin again --input \"%s\"", a440_stereo_pcm),
    RETURN_CODE_SUCCESS, true
  );
  runApplicationTest(environment, "Process MIDI with vstxsynth plugin",
    buildTestArgumentString("--plugin vstxsynth --midi-file \"%s\"", c_scale_mid),
    RETURN_CODE_SUCCESS, true
  );
  runApplicationTest(environment, "Process effect chain",
    buildTestArgumentString("--plugin vstxsynth,again --midi-file \"%s\"", c_scale_mid),
    RETURN_CODE_SUCCESS, true
  );
  runApplicationTest(environment, "Load FXP preset to VST",
    buildTestArgumentString("--plugin \"again,%s\" --input \"%s\"", again_test_fxp, a440_stereo_pcm),
    RETURN_CODE_SUCCESS, true
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
