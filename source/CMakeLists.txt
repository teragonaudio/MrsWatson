cmake_minimum_required(VERSION 2.6)
project(mrswatsoncore)

file(GLOB mrswatsoncore_SOURCES *.c */*.c */*.cpp)
add_library(mrswatsoncore ${mrswatsoncore_SOURCES})
