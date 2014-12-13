cmake_minimum_required(VERSION 2.6)
project(MrsWatsonTest)

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

add_executable(mrswatsontest
  ${mrswatsontest_SOURCES}
  ${mrswatsontest_HEADERS}
)
target_link_libraries(mrswatsontest
  mrswatsoncore
  audiofile
)

add_executable(mrswatsontest64
  ${mrswatsontest_SOURCES}
  ${mrswatsontest_HEADERS}
)
target_link_libraries(mrswatsontest64
  mrswatsoncore64
  audiofile64
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(mrswatsontest PROPERTIES COMPILE_FLAGS "-m32")
  set_target_properties(mrswatsontest PROPERTIES LINK_FLAGS "-m32")
  target_link_libraries(mrswatsontest dl)

  set_target_properties(mrswatsontest64 PROPERTIES COMPILE_FLAGS "-m64")
  set_target_properties(mrswatsontest64 PROPERTIES LINK_FLAGS "-m64")
  target_link_libraries(mrswatsontest64 dl)
elseif(APPLE)
  set_target_properties(mrswatsontest PROPERTIES OSX_ARCHITECTURES "i386")
  set_target_properties(mrswatsontest64 PROPERTIES OSX_ARCHITECTURES "x86_64")
elseif(MSVC)
  # On Unix, we can build both the 32/64 bit versions at once. However with
  # Visual Studio we need to generate two separate out-of-source build dirs,
  # one for each architecture.
  # TODO: There must be some way to fix this, Juce manages to do it...
  if(${PLATFORM_BITS} EQUAL 32)
    set_target_properties(mrswatsontest PROPERTIES COMPILE_FLAGS "/D WIN32=1")
  elseif(${PLATFORM_BITS} EQUAL 64)
    set_target_properties(mrswatsontest64 PROPERTIES COMPILE_FLAGS "/MACHINE:X64 /D WIN64=1")
  endif()
endif()

# The main executable must be built to correctly run integration tests
add_dependencies(mrswatsontest mrswatson)
add_dependencies(mrswatsontest64 mrswatson64)