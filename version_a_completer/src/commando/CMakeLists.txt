add_executable(server
        ../main_server.c
        server.c server.h
        pilot.c pilot.h
        robot.c robot.h
        commands_functions.c commands_functions.h)

target_link_libraries(server LINK_PUBLIC infox)
target_link_libraries(server LINK_PUBLIC robocom)