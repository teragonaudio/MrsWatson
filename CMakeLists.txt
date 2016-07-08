cmake_minimum_required(VERSION 3.0)
project(MrsWatson)

#################
# Build options #
#################

option(WITH_AUDIOFILE "Use libaudiofile for reading/writing audio files" ON)
option(WITH_FLAC "Support for FLAC files (requires libaudiofile)" OFF)
option(WITH_VST_SDK "Manually specify VST SDK zipfile" "")
option(VERBOSE "Show extra build information" OFF)


if(WITH_AUDIOFILE)
  add_definitions(-DUSE_AUDIOFILE=1)
endif()

if(WITH_FLAC)
  if(NOT WITH_AUDIOFILE)
    message(FATAL_ERROR "FLAC support requires WITH_AUDIOFILE")
  endif()
  add_definitions(-DUSE_FLAC=1)
endif()

############
# Platform #
############

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(mw_PLATFORM "linux")
  set(mw_PLATFORM_NAME "Linux")
elseif(APPLE)
  set(mw_PLATFORM "mac")
  set(mw_PLATFORM_NAME "Mac OS X")
elseif(WIN32)
  set(mw_PLATFORM "windows")
  set(mw_PLATFORM_NAME "Windows")
else()
  message(FATAL_ERROR "Unknown or unsupported platform")
endif()

#######################
# Bitness (word size) #
#######################

if(UNIX)
  set(mw_BUILD_32 ON)
  set(mw_BUILD_64 ON)
elseif(MSVC)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(mw_BUILD_32 OFF)
    set(mw_BUILD_64 ON)
  else()
    set(mw_BUILD_32 ON)
    set(mw_BUILD_64 OFF)
  endif()
endif()

###############
# Build Flags #
###############

if(MSVC)
  # We don't care about intdir, binary output path is set above
  set(CMAKE_CFG_INTDIR ".")

  set(CMAKE_C_FLAGS_DEBUG "/DDEBUG=1 /D_DEBUG /MTd /Ob0 /Od /RTC1")
  set(CMAKE_C_FLAGS_MINSIZEREL "/MT /O1 /Ob1 /Oi /DNDEBUG")
  set(CMAKE_C_FLAGS_RELEASE "/MT /O2 /Ob2 /Oi /DNDEBUG")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /DNDEBUG")

  set(CMAKE_CXX_FLAGS_DEBUG "/DDEBUG=1 /D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O1 /Ob1 /Oi /DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /Oi /DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /DNDEBUG")

  add_definitions("/W3 /WX /MP /D_CRT_SECURE_NO_WARNINGS=1 /DWINDOWS=1")
elseif(UNIX)
  add_definitions("-DUNIX=1")

  # Compiler flags common to C/C++ on all Unix platforms
  set(mw_COMMON_FLAGS_LIST
    "-fmessage-length=0"
    "-pipe"

    "-Werror"
    "-Waddress"
    "-Wchar-subscripts"
    "-Wcomment"
    "-Wformat"
    "-Wmissing-field-initializers"
    "-Wno-trigraphs"
    "-Wnonnull"
    "-Wparentheses"
    "-Wreturn-type"
    "-Wsequence-point"
    "-Wshadow"
    "-Wsign-compare"
    "-Wstrict-aliasing"
    "-Wstrict-overflow=1"
    "-Wswitch"
    "-Wswitch-default"
    "-Wtrigraphs"
    "-Wuninitialized"
    "-Wunused-label"
    "-Wunused-value"
    "-Wunused-variable"
    "-Wvolatile-register-var"
  )

  set(mw_CFLAGS_LIST
    "-Wenum-compare"
    "-Wimplicit-int"
    "-Wimplicit-function-declaration"
    "-Wmain"
    "-Wmissing-braces"
    "-Wpointer-sign"
  )

  set(mw_CPPFLAGS_LIST
    "-Wc++11-compat"
    "-Wreorder"
  )

    # Clang-specific flags
  if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(mw_COMMON_FLAGS_LIST
      ${mw_COMMON_FLAGS_LIST}
      "-Wunused-const-variable"
      "-Wunused-function"
    )

  # GCC-specific flags
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(mw_COMMON_FLAGS_LIST
      ${mw_COMMON_FLAGS_LIST}
      "-Wmaybe-uninitialized"
    )
  endif()

  # Linux-specific GCC flags
  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(mw_CFLAGS_LIST
      ${mw_CFLAGS_LIST}
      "-std=c99"
    )

    find_package(X11)
    find_library(X11 REQUIRED)

    add_definitions("-DLINUX=1")
    add_definitions("-D_POSIX_C_SOURCE=200809L")
    add_definitions("-D__cdecl=")
  endif()

  # Mac-specific compiler flags
  if(APPLE)
    set(mw_COMMON_FLAGS_LIST
      ${mw_COMMON_FLAGS_LIST}
      "-fpascal-strings"
      "-Wnewline-eof"
      "-Wshorten-64-to-32"
      "-fasm-blocks"
      "-mmacosx-version-min=10.5"
    )

    set(mw_LINKER_FLAGS_LIST
      "-framework AppKit"
      "-framework Carbon"
      "-framework CoreFoundation"
      "-framework Foundation"
    )

    if(${CMAKE_GENERATOR} MATCHES "Xcode")
      set(mw_LINKER_FLAGS_LIST
        ${mw_LINKER_FLAGS_LIST}
        "-stdlib=libstdc++"
      )
    endif()

    add_definitions("-DMACOSX=1")
    # Homebrew places installed libraries in /usr/local, but no major
    # Linux distro does that anymore (last time I checked...)
    include_directories("/usr/local/include")
  endif()

  # CMake multi-line strings are a pain to work with, so it's much easier to
  # define lists and then construct strings from them.
  string(REPLACE ";" " " mw_COMMON_FLAGS "${mw_COMMON_FLAGS_LIST}")
  string(REPLACE ";" " " mw_CFLAGS "${mw_CFLAGS_LIST}")
  string(REPLACE ";" " " mw_CPPFLAGS "${mw_CPPFLAGS_LIST}")
  string(REPLACE ";" " " mw_LINKER_FLAGS "${mw_LINKER_FLAGS_LIST}")

  set(mw_CFLAGS "${mw_COMMON_FLAGS} ${mw_CFLAGS}")
  set(mw_CPPFLAGS "${mw_COMMON_FLAGS} ${mw_CPPFLAGS}")

  # Set compiler flags from shared & platform-specific lists
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${mw_CFLAGS}")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${mw_CFLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${mw_CPPFLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${mw_CPPFLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${mw_LINKER_FLAGS}")
endif()

##################
# Subdirectories #
##################

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_SOURCE_DIR}/source)

set(mw_cmake_scripts_DIR ${PROJECT_SOURCE_DIR}/cmake)

# The vendor subdirectory must be defined first, since it sets some variables
# that are referenced by CMake scripts in the other directories
add_subdirectory(vendor)
add_subdirectory(source)
add_subdirectory(main)
add_subdirectory(test)

#############
# Packaging #
#############

include(${mw_cmake_scripts_DIR}/BuildPackage.cmake)

#################
# Build summary #
#################

if(VERBOSE)
  message(STATUS "Build configuration")
  get_directory_property(preprocessor_flags COMPILE_DEFINITIONS)
  message("   Preprocessor flags: ${preprocessor_flags}")
  message("   C Compiler: ${CMAKE_C_COMPILER}")
  message("   C Compiler flags: ${CMAKE_C_FLAGS}")
  message("   C Compiler flags (Debug): ${CMAKE_C_FLAGS_DEBUG}")
  message("   C Compiler flags (Release): ${CMAKE_C_FLAGS_RELEASE}")
  message("   C++ Compiler: ${CMAKE_CXX_COMPILER}")
  message("   C++ Compiler flags: ${CMAKE_CXX_FLAGS}")
  message("   C++ Compiler flags (Debug): ${CMAKE_CXX_FLAGS_DEBUG}")
  message("   C++ Compiler flags (Release): ${CMAKE_CXX_FLAGS_RELEASE}")
  message("   Linker: ${CMAKE_LINKER}")
  message("   Linker flags: ${CMAKE_SHARED_LINKER_FLAGS}")
  message("   Linker flags (Debug): ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
  message("   Linker flags (Release): ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
  if(CMAKE_BUILD_TYPE)
    message("   Build type: ${CMAKE_BUILD_TYPE}")
  else()
    message("   Build type: Set by IDE")
  endif()
  message("   Build 32-bit binary: ${mw_BUILD_32}")
  message("   Build 64-bit binary: ${mw_BUILD_64}")
  message("   Platform name: ${mw_PLATFORM_NAME}")
  message(STATUS "Options")
  message("   WITH_AUDIOFILE: ${WITH_AUDIOFILE}")
  message("   WITH_FLAC: ${WITH_FLAC}")
endif()
