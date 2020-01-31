cmake_minimum_required(VERSION 3.15)
project(MR_ROBOT C)

set(CMAKE_C_STANDARD 99)


file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "*.c")
add_executable(main ${SOURCES})

#include_directories(version_a_completer/src)
#add_executable(main version_a_completer/src/main.c)
#include_directories(infox_prose-x86_64-v0.3/include/infox/prose)
#find_library(INFOX_LIB libinfox.a infox_prose-x86_64-v0.3/lib)
#target_link_libraries(main LINK_PUBLIC ${INFOX_LIB})
