cmake_minimum_required(VERSION 3.15)

project(sServer LANGUAGES CXX)

include(../CMake/Common.cmake)
include(../CMake/ProjectOptions.cmake)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    # includes
    # include/Example.h

    # sources
    src/Main.cpp
)

# dll or excutable
BuildAsExecutable("${PROJECT_SOURCES}")
SetProjectPaths(${PROJECT_NAME} ${PROJECT_NAME})
SetTargetIncludes(${PROJECT_NAME})
SetProjectOptions(${PROJECT_NAME})

LinkLib(sCore PRIVATE)
LinkLib(sIPC PRIVATE)

# Startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
