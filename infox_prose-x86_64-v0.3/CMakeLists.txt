
#[[

    include/infox/prose/prose.h
    include/infox/prose/defs.h
    include/infox/prose/contact_sensor.h
    include/infox/prose/light_sensor.h
    include/infox/prose/motors.h
    include/infox/prose/prose_brick_pi.h
    include/infox/prose/prose_infox.h
]]

project(infox)

set(HEADERS
    include/infox/prose/prose.h
    include/infox/prose/defs.h
    include/infox/prose/contact_sensor.h
    include/infox/prose/light_sensor.h
    include/infox/prose/motors.h
    include/infox/prose/prose_brick_pi.h
    include/infox/prose/prose_infox.h
)
set(SRC lib/libinfox.a)

add_library(infox  ${SRC} )

target_include_directories(
        infox PUBLIC
        # Headers used from source/build location:
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        # Headers used from the installed location:
        "$<INSTALL_INTERFACE:include>"
)

install(TARGETS infox DESTINATION .)
install(FILES ${HEADERS} DESTINATION .)



#[[
add_library(infox
    include/infox/prose/prose.h
    include/infox/prose/defs.h
    include/infox/prose/contact_sensor.h
    include/infox/prose/light_sensor.h
    include/infox/prose/motors.h
    include/infox/prose/prose_brick_pi.h
    include/infox/prose/prose_infox.h)
]]

set_target_properties(infox PROPERTIES LINKER_LANGUAGE C)

# set_property(TARGET infox PROPERTY IMPORTED_LOCATION lib/libinfox.a)

target_include_directories(infox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/infox/prose)
