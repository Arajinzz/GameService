cmake_minimum_required(VERSION 3.15)

# Project name and language
project(GameService LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# vars
set(ENGINE_RUNTIME "MultiThreaded$<$<CONFIG:Debug>:Debug>")  # /MT or /MTd
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_BINARY_DIR}/WiEntry/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_BINARY_DIR}/WiEntry/Release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/WiEntry/RelWithDebInfo")
set(BIN_FOLDER "${CMAKE_BINARY_DIR}/WiEntry/$<IF:$<CONFIG:Debug>,Debug,$<IF:$<CONFIG:Release>,Release,RelWithDebInfo>>")
set(ENTRY_FOLDER "${CMAKE_BINARY_DIR}/WiEntry")
message(STATUS "Binary Folder: ${BIN_FOLDER}")

# Restrict configs in Visual Studio to Debug & Release
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)

#########################################################
#               FIRST PARTY                             #
#########################################################

add_library(PROJECT_OPTIONS INTERFACE)

# Assign necessary flags
target_compile_options(PROJECT_OPTIONS INTERFACE
    # both configs
    $<$<CXX_COMPILER_ID:MSVC>:/fp:precise /fp:except- /arch:AVX2 /GR- /EHsc /W4 /WX>

    # release
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2 /W0 /DNDEBUG>

    # release with debug info
    $<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:MSVC>>:/O2 /W0 /DNDEBUG>

    # debug
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/Od /Zi /DDEBUG>
)

# runtime
set_property(TARGET PROJECT_OPTIONS PROPERTY MSVC_RUNTIME_LIBRARY ${ENGINE_RUNTIME})

# preprocessors
target_compile_definitions(PROJECT_OPTIONS INTERFACE
    $<$<CONFIG:Release>:RELEASE_BUILD>
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:RelWithDebInfo>:RELEASE_BUILD>
)

add_subdirectory(WiApplication)
add_subdirectory(WiCore)
add_subdirectory(WiEntry)
add_subdirectory(WiSandbox)
add_subdirectory(WiBackendDX12)
add_subdirectory(WiRender)
add_subdirectory(WiWorld)

#########################################################
#               THIRD PARTY                             #
#########################################################
include(FetchContent)

######################## SPDLOG #########################

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.3
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog
)

set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(spdlog)

# match project runtime
set_property(TARGET spdlog PROPERTY MSVC_RUNTIME_LIBRARY ${ENGINE_RUNTIME})

######################## SPDLOG #########################

######################## Jolt   #########################

FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics"
        GIT_TAG "v5.5.0"
	SOURCE_SUBDIR "Build"
)

set(DOUBLE_PRECISION OFF CACHE BOOL "" FORCE)
set(CROSS_PLATFORM_DETERMINISTIC ON CACHE BOOL "" FORCE)
set(FLOATING_POINT_EXCEPTIONS_ENABLED OFF CACHE BOOL "" FORCE)
set(CPP_EXCEPTIONS_ENABLED OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(JoltPhysics)

######################## Jolt   #########################

# Startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT WiEntry)
