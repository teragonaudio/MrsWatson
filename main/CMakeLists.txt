cmake_minimum_required(VERSION 3.0)
project(MrsWatsonMain)

include(${mw_cmake_scripts_DIR}/ConfigureTarget.cmake)
set(main_SOURCES MrsWatsonMain.c)

function(add_main_target wordsize)
  if(${wordsize} EQUAL 32)
    set(main_target_NAME mrswatson)
  else()
    set(main_target_NAME mrswatson64)
  endif()

  add_executable(${main_target_NAME} ${main_SOURCES})
  target_link_libraries(${main_target_NAME} mrswatsoncore${wordsize})

  if(WITH_AUDIOFILE)
    target_link_libraries(${main_target_NAME} audiofile${wordsize})
    if(WITH_FLAC)
      target_link_libraries(${main_target_NAME} flac${wordsize})
    endif()
  endif()

  configure_target(${main_target_NAME} ${wordsize})
endfunction()

if(mw_BUILD_32)
  add_main_target(32)
endif()

if(mw_BUILD_64)
  add_main_target(64)
endif()
