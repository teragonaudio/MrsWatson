include_directories(${MRSWATSON_SOURCE_DIR}/source)
link_directories(${MRSWATSON_BINARY_DIR}/source)

add_executable(mrswatson main.c)
# TODO Needs to be platform specific flags
target_link_libraries(mrswatson MrsWatson)
