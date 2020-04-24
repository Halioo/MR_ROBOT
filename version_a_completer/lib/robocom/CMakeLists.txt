file(GLOB_RECURSE SRC *.c)
get_property(loc_LIB_DIR GLOBAL PROPERTY LIB_DIR)

add_library(robocom ${SRC})
target_include_directories(robocom PRIVATE ${loc_LIB_DIR})
set_target_properties(robocom PROPERTIES LINKER_LANGUAGE C)