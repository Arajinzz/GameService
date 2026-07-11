
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    # includes
    include/Example.h

    # sources
    src/Example.cpp
)

add_library(sIPC STATIC ${PROJECT_SOURCES})

target_include_directories(sIPC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES})

set_property(TARGET sIPC PROPERTY MSVC_RUNTIME_LIBRARY ${ENGINE_RUNTIME})
set_property(TARGET sIPC PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${BIN_FOLDER})

# link Core
target_link_libraries(sIPC PUBLIC sCore)
# link project options
target_link_libraries(sIPC PRIVATE PROJECT_OPTIONS)
