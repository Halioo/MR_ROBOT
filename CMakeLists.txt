cmake_minimum_required(VERSION 3.15)
project(MR_ROBOT C)

set(CMAKE_C_STANDARD 99)


file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "*.c")
add_executable(main ${SOURCES})

