# cmake version
cmake_minimum_required(VERSION 3.10)

# .sln's name
project(Learn-OpenGL-programming)

# rpavlik-cmake-modules
include(CreateLaunchers)
include(MSVCMultipleProcessCompile)

# 3rdlibs
include_directories(
	${3RD_LIBS_DIR}/glfw-3.3.4/include/
	${3RD_LIBS_DIR}/glad-0.1.34/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
    glfw
	GLAD_0134 # glad-0.1.34
)

add_definitions(
	-D_CRT_SECURE_NO_WARNINGS
)

# all .vcxproj
file(GLOB M_Tutorias "*")
foreach(PATH_NAME ${M_Tutorias})
	if(NOT IS_DIRECTORY ${PATH_NAME})
		continue()
	endif()

	# each <tutorialxx> build separate .sln
	file(RELATIVE_PATH DIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${PATH_NAME})
	get_filename_component(SOLUTION_NAME ${PATH_NAME} DIRECTORY)
	string(REPLACE "/" "\\" SOLUTION_NAME "${DIR_PATH}")
	set(M_SOURCE_DIR_FULL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SOLUTION_NAME}/")

	file(GLOB_RECURSE ALL_FILES
		"${SOLUTION_NAME}/include/*.*"
		"${SOLUTION_NAME}/src/*.*"
	)

	foreach(FILE ${ALL_FILES})
		get_filename_component(GET_DIR_PATH ${FILE} DIRECTORY)
		file(RELATIVE_PATH DIR_PATH "${M_SOURCE_DIR_FULL_PATH}" ${GET_DIR_PATH})
		string(REPLACE "/" "\\" DIR_NAME ${DIR_PATH})
		source_group(${DIR_NAME} FILES ${FILE})
	endforeach()

	add_executable(${SOLUTION_NAME} ${ALL_FILES})
	target_link_libraries(${SOLUTION_NAME} ${ALL_LIBS})
	target_include_directories(${SOLUTION_NAME} PRIVATE "${SOLUTION_NAME}/include")

	if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
		add_custom_command(
			TARGET ${SOLUTION_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${SOLUTION_NAME}${CMAKE_EXECUTABLE_SUFFIX}" "${M_SOURCE_DIR_FULL_PATH}"
		)
	endif()
endforeach()


