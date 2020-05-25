
file(GLOB_RECURSE SERVER_src "CommandoV3/*.c" "CommandoV3/*.h")
file(GLOB_RECURSE CLIENT_src "TelcoV3/*.c" "TelcoV3/*.h")

add_executable(client main_client.c
    ${CLIENT_src}
)
add_executable(server main_server.c
    ${SERVER_src}
        CommandoV3/logger.c CommandoV3/logger.h)


target_link_libraries(client
    infox robocom watchdog mailbox liste_chainee
    pthread rt
)
target_link_libraries(server
    infox robocom watchdog mailbox liste_chainee
    pthread rt
)


get_property(loc_INFOX_DIR GLOBAL PROPERTY INFOX_DIR)
get_property(loc_LIB_DIR GLOBAL PROPERTY LIB_DIR)


target_include_directories(client PUBLIC ${loc_INFOX_DIR})
target_include_directories(client PUBLIC ${loc_LIB_DIR})

target_include_directories(server PUBLIC ${loc_INFOX_DIR})
target_include_directories(server PUBLIC ${loc_LIB_DIR})
