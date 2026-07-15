include(../CMake/Common.cmake)

set(BIN_FOLDER 
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/$<IF:$<CONFIG:Debug>,Debug,$<IF:$<CONFIG:Release>,Release,RelWithDebInfo>>")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        $<TARGET_FILE_DIR:DLLRunner>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/DLLRunner
)

AddSubProject(DLLRunner)

SetDebugCommand(
    ${PROJECT_NAME}
    "${BIN_FOLDER}/DLLRunner/DLLRunner.exe"
    # define args after this
    # -test (for example)
)
