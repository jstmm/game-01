cmake_minimum_required(VERSION 3.28)

project(game
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable compiler-specific extensions
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(IMGUI_VERSION 1.91.7)

if(NOT CMAKE_BUILD_TYPE)
  # Set default build type to Debug
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Set build type to Debug or Release" FORCE)
endif()

find_package(raylib REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

# Adding our source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cc")
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include/")

# Declaring our executable
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})

if (UNIX)
  target_link_libraries(${PROJECT_NAME}
    raylib
    m
    pthread
    dl
    GL
    rt
    X11
  )
endif()

if (APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    m
    pthread
    dl
    "-framework IOKit"
    "-framework Cocoa"
    "-framework OpenGL"
  )
endif()

# Setting ASSETS_PATH
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/") # Set the asset path macro to the absolute path on the dev machine
else()
  target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable
endif()
