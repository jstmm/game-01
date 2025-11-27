cmake_minimum_required(VERSION 3.15)
project(game CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(raylib CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
        src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
)
