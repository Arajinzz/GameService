cmake_minimum_required(VERSION 3.15)
project(GameService LANGUAGES CXX)

option(BUILD_SERVER "Build the server" ON)
option(BUILD_CLIENT "Build the client" OFF)

if(BUILD_SERVER)
    set(EntryPoint sServer)
endif()

if(BUILD_CLIENT)
    set(EntryPoint sClient)
endif()

include(CMake/Common.cmake)
include(CMake/ProjectOptions.cmake)
include(CMake/OutputDirs.cmake)
include(CMake/Dependencies.cmake)

# vars
message(STATUS "Binary Folder: ${BIN_FOLDER}")

add_subdirectory(sCore)
add_subdirectory(sFramework)
add_subdirectory(sIPC)

if(BUILD_SERVER)
    add_subdirectory(sServer)
endif()

if(BUILD_CLIENT)
    add_subdirectory(sClient)
endif()

# Startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EntryPoint})
