link_directories(${CMAKE_SOURCE_DIR}/source)

file(GLOB MrsWatson_SOURCES *.c)
add_executable(mrswatson ${MrsWatson_SOURCES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux also requires this library for dlopen
  target_link_libraries(mrswatson MrsWatson dl)
else()
  target_link_libraries(mrswatson MrsWatson)
endif()

