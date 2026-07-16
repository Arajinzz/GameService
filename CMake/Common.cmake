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
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/$<IF:$<CONFIG:Debug>,Debug,$<IF:$<CONFIG:Release>,Release,RelWithDebInfo>>")
    set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY ${ENGINE_RUNTIME})
    set_property(TARGET ${target} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${BIN_FOLDER})
    target_link_libraries(${target} PUBLIC PROJECT_OPTIONS)
endfunction()

function(GetDependencies target result)
    set(_visited "")

    function(_collect_dependencies current)
        if(NOT TARGET ${current})
            return()
        endif()

        # Avoid infinite loops
        list(FIND _visited ${current} already_seen)
        if(NOT already_seen EQUAL -1)
            return()
        endif()

        list(APPEND _visited ${current})

        # Get normal linked libraries
        get_target_property(link_libs ${current} LINK_LIBRARIES)

        # Also get interface dependencies
        get_target_property(interface_libs ${current} INTERFACE_LINK_LIBRARIES)

        set(all_libs
            ${link_libs}
            ${interface_libs}
        )

        foreach(lib IN LISTS all_libs)
            # Skip generator expressions for now
            if(lib MATCHES "^\\$<")
                continue()
            endif()

            if(TARGET ${lib})
                list(APPEND _result ${lib})
                _collect_dependencies(${lib})
            endif()
        endforeach()

        set(_visited ${_visited} PARENT_SCOPE)
        set(_result ${_result} PARENT_SCOPE)
    endfunction()

    set(_result "")
    _collect_dependencies(${target})

    # Remove duplicates
    list(REMOVE_DUPLICATES _result)

    set(${result} ${_result} PARENT_SCOPE)
endfunction()

# this has a fatal flaw where, if dependency is changed, the source would not receive the new files
function(DistributeDLL source target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "Target ${target} does not exist")
    endif()

    get_target_property(type ${target} TYPE)

    if(NOT ("${type}" STREQUAL "EXECUTABLE"
        OR "${type}" STREQUAL "SHARED_LIBRARY"))
        return()
    endif()
    
    message(STATUS "${target} dlls will be distributed to ${source}")
    # transitive dlls, this has to ${source} unlike pdb, lib
    add_custom_command(TARGET ${source} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${source}>
            $<TARGET_FILE_DIR:${source}>
        COMMAND_EXPAND_LISTS
    )
    # not transitive
    GetDependencies(${source} deps)
    foreach(dep IN LISTS deps)
        if(NOT ("${type}" STREQUAL "EXECUTABLE"
            OR "${type}" STREQUAL "SHARED_LIBRARY"))
            continue()
        endif()
        if("${dep}" STREQUAL "PROJECT_OPTIONS")
            continue()
        endif()
        message(STATUS "${dep} pdb will be distributed to ${source}")
        add_custom_command(TARGET ${source} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_PDB_FILE:${dep}>
                $<TARGET_FILE_DIR:${source}>
            COMMAND_EXPAND_LISTS
        )
    endforeach()
    # not needed at runtime
    #add_custom_command(TARGET ${source} POST_BUILD
    #    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #        $<TARGET_LINKER_FILE:${target}>
    #        $<TARGET_FILE_DIR:${source}>
    #    COMMAND_EXPAND_LISTS
    #)
endfunction()

function(SymLinkLibs)
    if(NOT TARGET ${PROJECT_NAME})
        message(FATAL_ERROR "Target ${PROJECT_NAME} does not exist")
    endif()

    GetDependencies(${PROJECT_NAME} deps)

    set(SYMLINK_COMMANDS)
    foreach(dep IN LISTS deps)
        if("${dep}" STREQUAL "PROJECT_OPTIONS")
            continue()
        endif()
        get_target_property(type ${target} TYPE)
        if(NOT ("${type}" STREQUAL "EXECUTABLE"
            OR "${type}" STREQUAL "SHARED_LIBRARY"))
            continue()
        endif()

        list(APPEND SYMLINK_COMMANDS
            # create
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                "$<TARGET_FILE:${dep}>"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_NAME:${dep}>"
            # create PDB
            COMMAND
                $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:${CMAKE_COMMAND}>
                $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:-E>
                $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:create_symlink>
                $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:$<TARGET_PDB_FILE:${dep}>>
                $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:$<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_PDB_FILE_NAME:${dep}>>
        )
    endforeach()

    add_dependencies(${PROJECT_NAME}_Utilities ${PROJECT_NAME})
    add_custom_command(TARGET ${PROJECT_NAME}_Utilities POST_BUILD
        ${SYMLINK_COMMANDS}
        VERBATIM
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
    add_dependencies(${PROJECT_NAME} ${target})

    #DistributeDLL(${PROJECT_NAME} ${target})
    SymLinkLibs()
endfunction()

function(AddSubProject target)
    if(NOT TARGET ${target})
        add_subdirectory(
            ${CMAKE_CURRENT_LIST_DIR}/../${target}
            ${CMAKE_CURRENT_BINARY_DIR}/${target}
        )
    endif()
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

function(SetDebugCommand target command)
    set_target_properties(${target} PROPERTIES
        VS_DEBUGGER_COMMAND "${command}"
        VS_DEBUGGER_COMMAND_ARGUMENTS "${ARGN}"
    )
endfunction()
