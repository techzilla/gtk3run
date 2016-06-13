cmake_minimum_required(VERSION 3.5)
project(gtk3run
        VERSION 0.5.0
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic -rdynamic")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake/modules")
find_package(GTK3 REQUIRED)


add_subdirectory("${PROJECT_SOURCE_DIR}/src")



