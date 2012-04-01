cmake_minimum_required(VERSION 2.8.5)

project(cg)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/misc/cmake)
include(config)

enable_testing()

add_subdirectory(src)
