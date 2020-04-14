add_library(infox STATIC IMPORTED lib/libinfox.a)
target_include_directories(infox INTERFACE include/infox/prose)

set_target_properties(infox PROPERTIES LINKER_LANGUAGE C)
