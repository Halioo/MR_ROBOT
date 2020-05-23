
set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/lib/libinfox.a")
set(HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/infox/prose/")


set_property(GLOBAL PROPERTY INFOX_LIB ${SRC})
set_property(GLOBAL PROPERTY INFOX_DIR ${HEADERS})
