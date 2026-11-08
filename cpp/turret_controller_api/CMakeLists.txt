cmake_minimum_required(VERSION 3.22)

# Build turret controller API
add_library(tcapi STATIC)

target_sources(
    PRIVATE
        turret_controller_api.cpp
)

target_include_directories(tcapi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/hammerhead-turret-controller/htc/control/
)

target_link_libraries(tcapi
    PRIVATE
        socketcancpp
)
target_compile_definitions(tcapi
    PUBLIC
        $<$<CONFIG:Debug>:DEBUG>
)
