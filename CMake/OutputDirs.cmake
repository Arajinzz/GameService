set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_BINARY_DIR}/${EntryPoint}/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_BINARY_DIR}/${EntryPoint}/Release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/${EntryPoint}/RelWithDebInfo")
set(BIN_FOLDER 
    "${CMAKE_BINARY_DIR}/${EntryPoint}/$<IF:$<CONFIG:Debug>,Debug,$<IF:$<CONFIG:Release>,Release,RelWithDebInfo>>")
set(ENTRY_FOLDER "${CMAKE_BINARY_DIR}/${EntryPoint}")
