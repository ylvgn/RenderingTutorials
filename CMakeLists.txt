# cmake version
cmake_minimum_required(VERSION 3.10)

# .sln's name
project(Tutorials)

find_package(OpenGL)

# check: <path-to-build> != <path-to-source>
if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
# check: <path-to-source> whether contains space
if(CMAKE_SOURCE_DIR MATCHES " ")
	message( "Your <path-to-source> contains spaces. If you experience problems when compiling, this can be the cause.", ${CMAKE_SOURCE_DIR})
endif()
# check: <path-to-build> whether contains space
if(CMAKE_BINARY_DIR MATCHES " ")
	message( "Your <path-to-build> contains spaces. If you experience problems when compiling, this can be the cause.", ${CMAKE_BINARY_DIR})
endif()

# Compile 3rdlibs dependencies
set(3RD_LIBS_DIR "${CMAKE_SOURCE_DIR}/3rdlibs")
add_subdirectory (3rdlibs)

# https://github.com/rpavlik/cmake-modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/3rdlibs/rpavlik-cmake-modules-3933abd")
# ex: include(UseBackportedModules)

# Projects/Applications
add_subdirectory (Learn-OpenGL-programming)
add_subdirectory (Physically-Based-Rendering)
