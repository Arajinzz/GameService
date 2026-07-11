
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    # includes
    include/Example.h

    # sources
    src/Example.cpp
)

add_library(sCore STATIC ${PROJECT_SOURCES})

target_include_directories(sCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES})

set_property(TARGET sCore PROPERTY MSVC_RUNTIME_LIBRARY ${ENGINE_RUNTIME})
set_property(TARGET sCore PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${BIN_FOLDER})

# link project options
target_link_libraries(sCore PRIVATE PROJECT_OPTIONS)
