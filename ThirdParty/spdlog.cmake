if(TARGET spdlog)
    return()
endif()

include(../CMake/Common.cmake)
include(../CMake/ProjectOptions.cmake)

set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED ON CACHE BOOL "" FORCE)

add_subdirectory(
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/spdlog
    ${CMAKE_BINARY_DIR}/ThirdParty/spdlog
)

SetProjectPaths(spdlog spdlog)

set_property(TARGET spdlog PROPERTY MSVC_RUNTIME_LIBRARY ${PROJECT_RUNTIME})

LinkLib(spdlog::spdlog PUBLIC)
