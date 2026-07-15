cmake_minimum_required(VERSION 3.15)

project(GameService LANGUAGES CXX)

add_subdirectory(DLLRunner)
add_subdirectory(sCore)
add_subdirectory(sIPC)
add_subdirectory(sServer)
add_subdirectory(sClient)

# Startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sClient)
