cmake_minimum_required(VERSION 3.15)

project(sCore LANGUAGES CXX)

include(../CMake/Common.cmake)
include(../CMake/ProjectOptions.cmake)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    # includes
    include/sCore.h
    include/Job.h
    include/JobSystem.h
    include/Logging.h
    include/RingBuffer.h
    include/SingleInstance.h
    include/SpinLock.h
    include/Timer.h
    include/WorkerThread.h

    # sources
    src/sCore.cpp
    src/JobSystem.cpp
    src/Logging.cpp
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
