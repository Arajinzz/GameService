# guard for duplicate includes
if(TARGET PROJECT_OPTIONS)
    return()
endif()

add_library(PROJECT_OPTIONS INTERFACE)

set(PROJECT_RUNTIME "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")  # /Md or /MDd

# Assign necessary flags
target_compile_options(PROJECT_OPTIONS INTERFACE
    # both configs
    $<$<CXX_COMPILER_ID:MSVC>:/fp:precise /fp:except- /arch:AVX2 /GR- /EHsc /W4 /WX>

    # release
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2 /W0 /DNDEBUG>

    # release with debug info
    $<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:MSVC>>:/O2 /W0 /DNDEBUG>

    # debug
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/Od /Zi /DDEBUG>
)

# runtime
set_property(TARGET PROJECT_OPTIONS PROPERTY MSVC_RUNTIME_LIBRARY ${PROJECT_RUNTIME})

# preprocessors
target_compile_definitions(PROJECT_OPTIONS INTERFACE
    $<$<CONFIG:Release>:RELEASE_BUILD>
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:RelWithDebInfo>:RELEASE_BUILD>
)
