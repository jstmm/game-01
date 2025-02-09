cmake_minimum_required(VERSION 3.14)
project(game CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib 3.0 REQUIRED)
# pkg_check_modules(X11 REQUIRED x11)

add_executable(${PROJECT_NAME} src/main.cc)

# target_include_directories(${PROJECT_NAME} PRIVATE ${RAYLIB_INCLUDE_DIRS} ${X11_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
# target_link_libraries(${PROJECT_NAME} PRIVATE raylib ${X11_LIBRARIES} m pthread dl rt)
