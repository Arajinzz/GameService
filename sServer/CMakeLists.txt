
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    # includes
    include/Example.h

    # sources
    src/Example.cpp
)

add_library(sServer STATIC ${PROJECT_SOURCES})

target_include_directories(sServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES})

set_property(TARGET sServer PROPERTY MSVC_RUNTIME_LIBRARY ${ENGINE_RUNTIME})
set_property(TARGET sServer PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${BIN_FOLDER})

# link Core
target_link_libraries(sServer PUBLIC sCore)
# link project options
target_link_libraries(sServer PRIVATE PROJECT_OPTIONS)
