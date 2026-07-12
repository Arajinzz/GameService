if(TARGET JoltPhysics)
    return()
endif()

include(../CMake/Common.cmake)
include(../CMake/ProjectOptions.cmake)

set(DOUBLE_PRECISION OFF CACHE BOOL "" FORCE)
set(CROSS_PLATFORM_DETERMINISTIC ON CACHE BOOL "" FORCE)
set(FLOATING_POINT_EXCEPTIONS_ENABLED OFF CACHE BOOL "" FORCE)
set(CPP_EXCEPTIONS_ENABLED OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

add_subdirectory(
    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/JoltPhysics
    ${CMAKE_BINARY_DIR}/ThirdParty/JoltPhysics
)

SetProjectPaths(JoltPhysics JoltPhysics)

set_property(TARGET JoltPhysics PROPERTY MSVC_RUNTIME_LIBRARY ${PROJECT_RUNTIME})
target_link_libraries(${PROJECT_NAME} PUBLIC JoltPhysics)

DistributeDLL()
