
file(GLOB_RECURSE SERVER_src "CommandoV3/*.c" "CommandoV3/*.h")
file(GLOB_RECURSE CLIENT_src "TelcoV3/*.c" "TelcoV3/*.h")

add_executable(client main_client.c
    ${CLIENT_src}
)

add_executable(server main_server.c
    ${SERVER_src}
)


target_link_libraries(client robocom watchdog util pthread rt)
target_link_libraries(server robocom watchdog util pthread rt)
