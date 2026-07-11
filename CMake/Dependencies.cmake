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
set(SPDLOG_BUILD_SHARED ON CACHE BOOL "" FORCE)

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
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(JoltPhysics)

######################## Jolt   #########################