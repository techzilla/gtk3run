cmake_minimum_required(VERSION 3.5)

project(gtk3run
        VERSION 0.5.0
        LANGUAGES C)

## Compiler Flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic -rdynamic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O2")


## Module Include
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" "${CMAKE_MODULE_PATH}")

## Force Out Of Tree
include(MacroOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out tree build.")

##
find_package(GTK3 REQUIRED)

##
add_subdirectory("${PROJECT_SOURCE_DIR}/src")


## Documentation
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if (BUILD_DOCUMENTATION)
    if (NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif ()
    add_subdirectory(doc)
endif ()
