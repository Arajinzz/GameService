cmake_minimum_required(VERSION 3.15)

project(sService LANGUAGES CXX)

include(../CMake/Common.cmake)
include(../CMake/ProjectOptions.cmake)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    # includes
    include/Utils.h
    include/Service.h
    include/${PROJECT_NAME}.h

    # sources
    src/Service.cpp
    src/${PROJECT_NAME}.cpp
)

# dll or excutable
BuildAsShared("${PROJECT_SOURCES}")
SetProjectPaths(${PROJECT_NAME} ${PROJECT_NAME})
SetTargetIncludes(${PROJECT_NAME})
SetProjectOptions(${PROJECT_NAME})
CreateUtilitiesProject()

# configure dll runner
include(../CMake/DLLRunner.cmake)

# link libraries
LinkLib(sCore PRIVATE)
LinkLib(sIPC PRIVATE)

# link thirdparty
include(../ThirdParty/spdlog.cmake)

# activate entrypoint
# AddDefine(ENTRY_POINT)

# Startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
