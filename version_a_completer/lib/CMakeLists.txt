set_property(GLOBAL PROPERTY LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")


add_subdirectory(robocom)
add_subdirectory(watchdog)
add_subdirectory(mailbox)


get_property(loc_INFOX_LIB GLOBAL PROPERTY INFOX_LIB)
get_property(loc_INFOX_DIR GLOBAL PROPERTY INFOX_DIR)

add_library(infox INTERFACE)
target_link_libraries(infox INTERFACE ${loc_INFOX_LIB})
target_include_directories(infox INTERFACE ${loc_INFOX_DIR})
target_compile_definitions(infox INTERFACE "-DENABLE_INFOX")