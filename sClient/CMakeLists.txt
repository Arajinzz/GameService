cmake_minimum_required(VERSION 3.15)

project(sClient LANGUAGES CXX)

include(../CMake/Common.cmake)
include(../CMake/ProjectOptions.cmake)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    # includes
    include/sClient.h

    # sources
    src/sClient.cpp
)

# dll or excutable
BuildAsShared("${PROJECT_SOURCES}")
SetProjectPaths(${PROJECT_NAME} ${PROJECT_NAME})
SetTargetIncludes(${PROJECT_NAME})
SetProjectOptions(${PROJECT_NAME})
CreateUtilitiesProject()

# configure dll runner
include(../CMake/DLLRunner.cmake)

LinkLib(sCore PRIVATE)
LinkLib(sIPC PRIVATE)
LinkLib(sServer PRIVATE)

# link thirdparty
include(../ThirdParty/spdlog.cmake)

# activate entrypoint
AddDefine(ENTRY_POINT)

# Startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
