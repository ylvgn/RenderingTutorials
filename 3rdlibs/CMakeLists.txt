if(MSVC AND NOT "${MSVC_VERSION}" LESS 1400)
	add_definitions( "/MP" )
endif()

add_definitions(
	-D_CRT_SECURE_NO_WARNINGS
)

### GLFW ###
add_subdirectory (glfw-3.3.4)

include_directories(
	glfw-3.3.4/include/GLFW/
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(OPENGL_LIBRARY
		${OPENGL_LIBRARY}
		-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl
		${CMAKE_DL_LIBS}
		${GLFW_LIBRARIES}
	)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(OPENGL_LIBRARY
		${OPENGL_LIBRARY}
		${CMAKE_DL_LIBS}
		${GLFW_LIBRARIES}
	)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


### GLAD ###
set(GLAD "glad-0.1.34")
# add_subdirectory (glad-0.1.34) #cmake and compile GLAD manually then got 'glad.c'

include_directories(
	"${GLAD}/include"
)

set(GLAD_SOURCE
	"${GLAD}/src/glad.c"
)

add_library(GLAD_0134 STATIC
	${GLAD_SOURCE}
)