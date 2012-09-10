include_directories(${MRSWATSON_SOURCE_DIR}/source)
link_directories(${MRSWATSON_BINARY_DIR}/source)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "-fmessage-length=0 -m32 -pipe -Wno-trigraphs -std=c99 -O3 -Wmissing-field-initializers -Wall -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -D__cdecl=\"\"")
  set(CMAKE_EXE_LINKER_FLAGS "-m32 -ldl")
  add_definitions(-DLINUX=1)
endif()

add_executable(mrswatson main.c ../source/libMrsWatson.a)
# TODO Needs to be platform specific flags
target_link_libraries(mrswatson)
