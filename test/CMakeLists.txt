cmake_minimum_required(VERSION 3.0)
project(MrsWatsonTest)

include(${CMAKE_SOURCE_DIR}/cmake/ConfigureTarget.cmake)

###########
# Sources #
###########

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

#################
# Source Groups #
#################

source_group(analysis ".*/analysis/.*")
source_group(app ".*/app/.*")
source_group(audio ".*/audio/.*")
source_group(base ".*/base/.*")
source_group(io ".*/io/.*")
source_group(midi ".*/midi/.*")
source_group(plugin ".*/plugin/.*")
source_group(time ".*/time/.*")
source_group(unit ".*/unit/.*")

##########
# Target #
##########

function(add_test_target wordsize)
  if(${wordsize} EQUAL 32)
    set(main_target_NAME mrswatson)
    set(test_target_NAME mrswatsontest)
  else()
    set(main_target_NAME mrswatson64)
    set(test_target_NAME mrswatsontest64)
  endif()

  add_executable(${test_target_NAME} ${test_SOURCES} ${test_HEADERS})
  target_link_libraries(${test_target_NAME} mrswatsoncore${wordsize})

  if(WITH_AUDIOFILE)
    target_link_libraries(${test_target_NAME} audiofile${wordsize})
    if(WITH_FLAC)
      target_link_libraries(${test_target_NAME} flac${wordsize})
    endif()
  endif()

  configure_target(${test_target_NAME} ${wordsize})

  # The main executable must be built to run the integration tests
  add_dependencies(${test_target_NAME} ${main_target_NAME})
endfunction()

if(mw_BUILD_32)
  add_test_target(32)
endif()

if(mw_BUILD_64)
  add_test_target(64)
endif()
