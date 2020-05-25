file(GLOB_RECURSE SRC *.c)
get_property(loc_LIB_DIR GLOBAL PROPERTY LIB_DIR)

add_library(liste_chainee ${SRC})
target_include_directories(liste_chainee PRIVATE ${loc_LIB_DIR})
set_target_properties(liste_chainee PROPERTIES LINKER_LANGUAGE C)