include(../CMake/Common.cmake)

set(BIN_FOLDER 
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/$<IF:$<CONFIG:Debug>,Debug,$<IF:$<CONFIG:Release>,Release,RelWithDebInfo>>")

AddSubProject(DLLRunner)

add_custom_command(TARGET ${PROJECT_NAME}_Utilities POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/DLLRunner"
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        "$<TARGET_FILE_DIR:DLLRunner>"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/DLLRunner"
    VERBATIM
)

SetDebugCommand(
    ${PROJECT_NAME}
    "${BIN_FOLDER}/DLLRunner/DLLRunner.exe"
    # define args after this
    "--dll ../${PROJECT_NAME}.dll"
)
