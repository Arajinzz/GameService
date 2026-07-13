cmake_minimum_required(VERSION 3.15)

project(sIPC LANGUAGES CXX)

include(../CMake/Common.cmake)
include(../CMake/ProjectOptions.cmake)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    # includes
    include/sIPC.h
    include/IdGenerator.h
    include/SharedMemory.h

    # sources
    src/sIPC.cpp
)

# dll or excutable
BuildAsShared("${PROJECT_SOURCES}")
SetProjectPaths(${PROJECT_NAME} ${PROJECT_NAME})
SetTargetIncludes(${PROJECT_NAME})
SetProjectOptions(${PROJECT_NAME})

# link libraries
LinkLib(sCore PUBLIC)
