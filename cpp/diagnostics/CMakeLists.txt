cmake_minimum_required(VERSION 3.22)

set(DIAGNOSTICS_PROJECT_NAME ${CMAKE_PROJECT_NAME}-diagnostics)

# Build diagnostics application
add_executable(${DIAGNOSTICS_PROJECT_NAME})
target_include_directories(${DIAGNOSTICS_PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
target_sources(${DIAGNOSTICS_PROJECT_NAME}
    PRIVATE
        main.cpp
)
target_link_libraries(${DIAGNOSTICS_PROJECT_NAME}
    PRIVATE
        socketcancpp
)
target_compile_definitions(${DIAGNOSTICS_PROJECT_NAME}
    PUBLIC
        $<$<CONFIG:Debug>:DEBUG>
)
