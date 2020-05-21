
set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/lib/libinfox.a")

add_library(infox STATIC ${SRC})
#set_target_properties(infox PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/")
set_target_properties(infox PROPERTIES LINKER_LANGUAGE C)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/lib/")

set_property(GLOBAL PROPERTY INFOX_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libinfox.a")

set_property(GLOBAL PROPERTY INFOX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/infox/prose/")
