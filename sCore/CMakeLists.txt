cmake_minimum_required(VERSION 3.15)

project(sCore LANGUAGES CXX)

include(../CMake/Common.cmake)
include(../CMake/ProjectOptions.cmake)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    # includes
    include/sCore.h

    # sources
    src/sCore.cpp
)

# dll or excutable
BuildAsShared("${PROJECT_SOURCES}")
SetProjectPaths(${PROJECT_NAME} ${PROJECT_NAME})
SetTargetIncludes(${PROJECT_NAME})
SetProjectOptions(${PROJECT_NAME})

# link libraries
#LinkLib(newLib PRIVATE)

#link thirdparty
include(../ThirdParty/spdlog.cmake)
