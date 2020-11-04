cmake_minimum_required (VERSION 3.10.2)
project (HELLO)

set( LWS_WITH_SSL OFF CACHE INTERNAL "" )

add_subdirectory(libwebsockets)

add_executable (helloDemo
  main.c
)

target_include_directories (helloDemo PUBLIC libwebsockets/lib)
target_include_directories (helloDemo PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/libwebsockets)
target_link_libraries (helloDemo LINK_PUBLIC websockets)
