cmake_minimum_required(VERSION 3.22)

# Build socketcan wrapper
add_library(socketcancpp STATIC)

target_sources(socketcancpp
    PRIVATE
        socketcan.cpp
)

target_include_directories(socketcancpp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(socketcancpp
    PRIVATE
        socketcan
        uring
)
target_compile_definitions(socketcancpp
    PUBLIC
        $<$<CONFIG:Debug>:DEBUG>
)
