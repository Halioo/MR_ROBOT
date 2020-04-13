add_executable(client
        main_client.c
        client.c client.h
        remoteui.c remoteui.h)

target_link_libraries(client robocom)
target_link_libraries(client LINK_PUBLIC infox)