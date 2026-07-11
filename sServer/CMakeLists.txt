
#file(GLOB_RECURSE PROJECT_INCLUDES CONFIGURE_DEPENDS
#    
#)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    src/Main.cpp
)

# Add sources and includes
add_executable(sServer ${PROJECT_SOURCES})
#target_sources(sServer PRIVATE ${PROJECT_INCLUDES})

# Add includes
target_include_directories(sServer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Organize filters in VS according to actual folders
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES})
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_INCLUDES})

set_property(TARGET sServer PROPERTY MSVC_RUNTIME_LIBRARY ${ENGINE_RUNTIME})
set_property(TARGET sServer PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${BIN_FOLDER})

target_link_libraries(sServer PRIVATE sCore)
# link project options
target_link_libraries(sServer PRIVATE PROJECT_OPTIONS)
