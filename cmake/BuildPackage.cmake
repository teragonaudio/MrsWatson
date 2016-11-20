cmake_minimum_required(VERSION 3.2)

function(add_dummy_package_target wordsize)
  add_custom_target(build_package_${wordsize}
    COMMAND ${CMAKE_COMMAND} -E echo "Skipping package for ${wordsize}-bit"
  )
endfunction()

function(add_package_target platform wordsize)
  set(pkg_NAME "MrsWatson-${mw_VERSION}-${platform}-${wordsize}bit")
  set(pkg_DIR "${CMAKE_BINARY_DIR}/${pkg_NAME}")

  if(${wordsize} EQUAL 32)
    set(mw_target "mrswatson")
  else()
    set(mw_target "mrswatson64")
  endif()

  if(MSVC)
    set(mw_exe_name "${mw_target}.exe")
  else()
    set(mw_exe_name "${mw_target}")
  endif()

  add_custom_target(build_package_${wordsize}
    COMMAND ${CMAKE_COMMAND} -E echo "Creating directories"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${pkg_DIR}"

    COMMAND ${CMAKE_COMMAND} -E echo "Copying documentation"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/LICENSE.txt" "${pkg_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/README.md" "${pkg_DIR}/README.txt"

    COMMAND ${CMAKE_COMMAND} -E echo "Copying executables"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/main/${CMAKE_BUILD_TYPE}/${mw_exe_name}" "${pkg_DIR}"

    COMMAND ${CMAKE_COMMAND} -E echo "Creating zipfile"
    COMMAND ${CMAKE_COMMAND} -E tar "cvf" "${CMAKE_BINARY_DIR}/${pkg_NAME}.zip" --format=zip "${pkg_DIR}"

    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )

  add_dependencies(build_package_${wordsize} ${mw_target})
endfunction()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_package_target("Linux" 32 "mrswatson")
  add_package_target("Linux" 64 "mrswatson64")
elseif(APPLE)
  add_package_target("Mac" 32)
  add_package_target("Mac" 64)
elseif(MSVC)
  if(mw_BUILD_32)
    add_package_target("Win" 32 "mrswatson")
    add_dummy_package_target(64)
  else()
    add_package_target("Win" 64 "mrswatson64")
    add_dummy_package_target(32)
  endif()
endif()
