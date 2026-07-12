include(GenerateExportHeader)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Restrict configs in Visual Studio to Debug & Release
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)

function(SetProjectPaths target projectName)
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG
            "${CMAKE_BINARY_DIR}/${projectName}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE
            "${CMAKE_BINARY_DIR}/${projectName}/Release"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO
            "${CMAKE_BINARY_DIR}/${projectName}/RelWithDebInfo"

        ARCHIVE_OUTPUT_DIRECTORY_DEBUG
            "${CMAKE_BINARY_DIR}/${projectName}/Debug"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE
            "${CMAKE_BINARY_DIR}/${projectName}/Release"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO
            "${CMAKE_BINARY_DIR}/${projectName}/RelWithDebInfo"
    )
endfunction()

function(SetTargetIncludes target)
    target_include_directories(${target} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

function(SetProjectOptions target)
    set(BIN_FOLDER 
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/$<IF:$<CONFIG:Debug>,Debug,$<IF:$<CONFIG:Release>,Release,RelWithDebInfo>>")
    set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY ${ENGINE_RUNTIME})
    set_property(TARGET ${target} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${BIN_FOLDER})
    target_link_libraries(${target} PUBLIC PROJECT_OPTIONS)
endfunction()

function(DistributeDLL target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "Target ${target} does not exist")
    endif()

    get_target_property(type ${target} TYPE)

    if(NOT ("${type}" STREQUAL "EXECUTABLE"
        OR "${type}" STREQUAL "SHARED_LIBRARY"))
        return()
    endif()

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${target}>
            $<TARGET_FILE_DIR:${target}>
        COMMAND_EXPAND_LISTS
    )
endfunction()

function(LinkLib target visibility)
    if(NOT TARGET ${target})
        add_subdirectory(
            ${CMAKE_CURRENT_LIST_DIR}/../${target}
            ${CMAKE_CURRENT_BINARY_DIR}/${target}
        )
    endif()

    if(TARGET ${target})
        message(STATUS "${target} exists")
    else()
        message(FATAL_ERROR "${target} does NOT exist")
    endif()

    target_link_libraries(${PROJECT_NAME} ${visibility} ${target})

    DistributeDLL(${PROJECT_NAME})
endfunction()

function(BuildAsShared PROJECT_SOURCES)
    if(TARGET ${PROJECT_NAME})
        return()
    endif()

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES})
    add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})
    generate_export_header(${PROJECT_NAME})
    message(STATUS "Building as shared library ...")
endfunction()

function(BuildAsExecutable PROJECT_SOURCES)
    if(TARGET ${PROJECT_NAME})
        return()
    endif()

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES})
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    message(STATUS "Building as executable ...")
endfunction()
