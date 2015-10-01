cmake_minimum_required(VERSION 2.6)
project(MrsWatsonMain)

include(${cmake_SCRIPTS_DIR}/ConfigureTarget.cmake)

set(mrswatsonmain_SOURCES MrsWatsonMain.c)
set(mrswatsonmain_LIBS mrswatsoncore)
set(mrswatsonmain_64_LIBS mrswatsoncore64)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(mrswatsonmain_LIBS ${mrswatsonmain_LIBS} dl)
  set(mrswatsonmain_64_LIBS ${mrswatsonmain_64_LIBS} dl X11)
endif()

if(WITH_AUDIOFILE)
  set(mrswatsonmain_LIBS ${mrswatsonmain_LIBS} audiofile)
  set(mrswatsonmain_64_LIBS ${mrswatsonmain_64_LIBS} audiofile64)
endif()

# Main executable target and associated libraries
add_executable(mrswatson ${mrswatsonmain_SOURCES})
target_link_libraries(mrswatson ${mrswatsonmain_LIBS})
add_executable(mrswatson64 ${mrswatsonmain_SOURCES})
target_link_libraries(mrswatson64 ${mrswatsonmain_64_LIBS})

configure_target(mrswatson 32)
configure_target(mrswatson64 64)
