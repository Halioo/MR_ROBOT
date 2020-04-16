add_library(util util.h)
set_target_properties(util PROPERTIES LINKER_LANGUAGE C)
target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(robocom robocom/robocom.c robocom/robocom.h)
set_target_properties(robocom PROPERTIES LINKER_LANGUAGE C)
target_include_directories(robocom PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/robocom/)

add_library(watchdog watchdog/watchdog.c watchdog/watchdog.h)
set_target_properties(watchdog PROPERTIES LINKER_LANGUAGE C)
target_include_directories(watchdog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/watchdog)

#[[
file(GLOB_RECURSE lib_files
        util.h
        robocom/robocom.c robocom/robocom.h
        watchdog/watchdog.c watchdog/watchdog.h)
]]
