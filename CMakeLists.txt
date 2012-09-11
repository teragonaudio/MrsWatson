cmake_minimum_required(VERSION 2.6)
project(MRSWATSON)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/vendor/vstsdk2.4/pluginterfaces/vst2.x)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "-fmessage-length=0 -m32 -pipe -Wno-trigraphs -std=c99 -O3 -Wmissing-field-initializers -Wall -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -D__cdecl=\"\"")
  set(CMAKE_EXE_LINKER_FLAGS "-m32 -ldl")
  add_definitions(-DLINUX=1)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_C_FLAGS "-arch i386 -fmessage-length=0 -pipe -Wno-trigraphs -fpascal-strings -std=c99 -O3 -Wmissing-field-initializers -Wall -Wmissing-prototypes -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -Wnewline-eof -Wshorten-64-to-32 -fasm-blocks -mmacosx-version-min=10.4")
  set(CMAKE_CXX_FLAGS "-arch i386 -fmessage-length=0 -pipe -Wno-trigraphs -fpascal-strings -O3 -Wmissing-field-initializers -Wall -Wmissing-prototypes -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -Wnewline-eof -Wshorten-64-to-32 -fasm-blocks -mmacosx-version-min=10.4")
  set(CMAKE_EXE_LINKER_FLAGS "-arch i386 -framework Carbon -framework CoreFoundation")
  add_definitions(-DMACOSX=1)
endif()

add_subdirectory(source)
add_subdirectory(main)
