cmake_minimum_required(VERSION 3.0)
project(MrsWatsonTest)

include(${cmake_SCRIPTS_DIR}/ConfigureTarget.cmake)

set(test_SOURCES
  MrsWatsonTestMain.c
  IntegrationTests.c
  analysis/AnalysisClipping.c
  analysis/AnalysisClippingTest.c
  analysis/AnalysisDistortion.c
  analysis/AnalysisDistortionTest.c
  analysis/AnalysisSilence.c
  analysis/AnalysisSilenceTest.c
  analysis/AnalyzeFile.c
  app/ProgramOptionTest.c
  audio/AudioSettingsTest.c
  audio/PcmSampleBufferTest.c
  audio/SampleBufferTest.c
  base/CharStringTest.c
  base/EndianTest.c
  base/FileTest.c
  base/LinkedListTest.c
  base/PlatformInfoTest.c
  io/SampleSourceTest.c
  midi/MidiSequenceTest.c
  midi/MidiSourceTest.c
  plugin/PluginChainTest.c
  plugin/PluginMock.c
  plugin/PluginPresetMock.c
  plugin/PluginPresetTest.c
  plugin/PluginTest.c
  plugin/PluginVst2xIdTest.c
  time/AudioClockTest.c
  time/TaskTimerTest.c
  unit/ApplicationRunner.c
  unit/TestRunner.c
  unit/UnitTests.c
)

set(test_HEADERS
  MrsWatsonTestMain.h
  analysis/AnalysisClipping.h
  analysis/AnalysisDistortion.h
  analysis/AnalysisSilence.h
  analysis/AnalyzeFile.h
  plugin/PluginMock.h
  plugin/PluginPresetMock.h
  unit/ApplicationRunner.h
  unit/TestRunner.h
)

source_group(analysis ".*/analysis/.*")
source_group(app ".*/app/.*")
source_group(audio ".*/audio/.*")
source_group(base ".*/base/.*")
source_group(io ".*/io/.*")
source_group(midi ".*/midi/.*")
source_group(plugin ".*/plugin/.*")
source_group(time ".*/time/.*")
source_group(unit ".*/unit/.*")

if(mw_BUILD_32)
  add_executable(mrswatsontest ${test_SOURCES} ${test_HEADERS})
  target_link_libraries(mrswatsontest mrswatsoncore)
  if(WITH_AUDIOFILE)
    target_link_libraries(mrswatsontest audiofile)
  endif()
  if(WITH_FLAC)
    target_link_libraries(mrswatsontest flac)
  endif()

  configure_target(mrswatsontest 32)
  # The main executable must be built to correctly run integration tests
  add_dependencies(mrswatsontest mrswatson)
endif()

if(mw_BUILD_64)
  add_executable(mrswatsontest64 ${test_SOURCES} ${test_HEADERS})
  target_link_libraries(mrswatsontest64 mrswatsoncore64)
  if(WITH_AUDIOFILE)
    target_link_libraries(mrswatsontest64 audiofile64)
  endif()
  if(WITH_FLAC)
    target_link_libraries(mrswatsontest64 flac64)
  endif()

  configure_target(mrswatsontest64 64)
  # The main executable must be built to correctly run integration tests
  add_dependencies(mrswatsontest64 mrswatson64)
endif()
