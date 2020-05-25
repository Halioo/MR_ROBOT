cmake_minimum_required(VERSION 3.10)
project(MR_ROBOT C)
set(CMAKE_C_STANDARD 99)

#add_compile_definitions(NDEBUG)

add_subdirectory(infox_prose-x86_64-v0.3)
add_subdirectory(version_a_completer/lib)
add_subdirectory(version_a_completer/src)
