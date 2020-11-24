cmake_minimum_required(VERSION 3.10.2)
project(SERVER_CLIENT_EXAMPLE)


#
# libwebsockets
#

set(LWS_WITH_SSL OFF CACHE INTERNAL "")
set(LWS_WITH_SDEVENT ON CACHE INTERNAL "")
set(LWS_WITH_EVLIB_PLUGINS OFF CACHE INTERNAL "")
add_subdirectory(libwebsockets)


#
# server app
#   will bind to port 8080 (user space http)
#   will wail for client connections
#   will send messages to clients
#

add_executable(server
        server_main.c
        )

target_include_directories(server PUBLIC libwebsockets/include)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/libwebsockets)
target_link_libraries(server LINK_PUBLIC websockets)


#
# client app
#   will connect to port 8080 (user space http)
#   will wait for & display server messages
#

add_executable(client
        client_main.c
        )

target_include_directories(client PUBLIC libwebsockets/include)
target_include_directories(client PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/libwebsockets)
target_link_libraries(client LINK_PUBLIC websockets)
