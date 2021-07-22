cmake_minimum_required(VERSION 3.10)

# ${SOLUTION_NAME}       = tutorial07_blend_textures
# M_SOURCE_DIR_FULL_PATH = proj/Learn-OpenGL-programming/tutorial07_blend_textures/

project(${SOLUTION_NAME})

include_directories(
    "include"
    .
)

file(GLOB_RECURSE ALL_FILES
    "shaders/*.*"
    "include/*.*"
    "src/*.*"
)

set(RESOURCE_FILES
    container.jpg
    wall.jpg
)

foreach(FILE ${ALL_FILES})
    get_filename_component(GET_DIR_PATH ${FILE} DIRECTORY)
    file(RELATIVE_PATH DIR_PATH "${M_SOURCE_DIR_FULL_PATH}" ${GET_DIR_PATH})
    string(REPLACE "/" "\\" DIR_NAME ${DIR_PATH})
    source_group(${DIR_NAME} FILES ${FILE})
endforeach()

add_executable(${SOLUTION_NAME} ${ALL_FILES} ${RESOURCE_FILES})
target_link_libraries(${SOLUTION_NAME} ${ALL_LIBS})

# set properties: debugging working directories
set_property(TARGET ${SOLUTION_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${M_SOURCE_DIR_FULL_PATH}")

# copy .exe to source dir
if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
    add_custom_command(
        TARGET ${SOLUTION_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${SOLUTION_NAME}${CMAKE_EXECUTABLE_SUFFIX}" "${M_SOURCE_DIR_FULL_PATH}"
    )
endif()

# set .vcxproj to FOLDER "Learn-OpenGL-programming"
set_target_properties(${SOLUTION_NAME} PROPERTIES FOLDER "Learn-OpenGL-programming")