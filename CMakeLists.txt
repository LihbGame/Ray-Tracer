cmake_minimum_required (VERSION 3.0)
project (RayTracing VERSION 0.1.0)

file(GLOB RAY_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/RayTracing/Src/*.cpp)
file(GLOB RAY_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/RayTracing/Head/*.h)
file(GLOB RAY_RESOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/RayTracing/Resource/*.*)

include(CMakeDependentOption)

if(MSVC)
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()

    add_definitions(/W0)
endif(MSVC)

add_definitions(-DUNICODE -D_UNICODE)



source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${RAY_SOURCES} ${RAY_HEADERS} ${RAY_RESOURCES})

include_directories(./RayTracing/Head)

add_executable(RayTracing WIN32 ${RAY_SOURCES} ${RAY_HEADERS} ${RAY_RESOURCES})

add_custom_command(TARGET RayTracing POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/RayTracing/Resource
                       ${CMAKE_SOURCE_DIR}/build/RayTracing/Resource)