cmake_minimum_required(VERSION 2.8.11)
project(MrsWatsonTest)

include(${cmake_SCRIPTS_DIR}/ConfigureTarget.cmake)

set(mrswatsontest_SOURCES
  MrsWatsonTestMain.c
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
  unit/IntegrationTests.c
  unit/TestRunner.c
  unit/UnitTests.c
)

set(mrswatsontest_HEADERS
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

set(mrswatsontest_LIBS mrswatsoncore)
set(mrswatsontest_64_LIBS mrswatsoncore64)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(mrswatsontest_LIBS ${mrswatsontest_LIBS} dl)
  set(mrswatsontest_64_LIBS ${mrswatsontest_64_LIBS} dl)
endif()

if(WITH_AUDIOFILE)
  set(mrswatsontest_LIBS ${mrswatsontest_LIBS} audiofile)
  set(mrswatsontest_64_LIBS ${mrswatsontest_64_LIBS} audiofile64)

  if(WITH_FLAC)
    set(mrswatsontest_LIBS ${mrswatsontest_LIBS} flac)
    set(mrswatsontest_64_LIBS ${mrswatsontest_64_LIBS} flac64)
  endif()
endif()

add_executable(mrswatsontest ${mrswatsontest_SOURCES} ${mrswatsontest_HEADERS})
target_link_libraries(mrswatsontest ${mrswatsontest_LIBS})
add_executable(mrswatsontest64 ${mrswatsontest_SOURCES} ${mrswatsontest_HEADERS})
target_link_libraries(mrswatsontest64 ${mrswatsontest_64_LIBS})

configure_target(mrswatsontest 32)
configure_target(mrswatsontest64 64)

# The main executable must be built to correctly run integration tests
add_dependencies(mrswatsontest mrswatson)
add_dependencies(mrswatsontest64 mrswatson64)
