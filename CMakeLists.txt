cmake_minimum_required(VERSION 3.28)
project(game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable compiler-specific extensions

if(NOT CMAKE_BUILD_TYPE)
  # Set default build type to Debug
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Set build type to Debug or Release" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB sources "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(game ${sources})

# target_include_directories(game PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(game
  raylib
  GL
  m
  pthread
  dl
  rt
  X11
)
