#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <my_lib.h>
#include <iostream>
using namespace std;
//-------------------------------------------------------------------
const int SCR_WIDTH = 800, SCR_HEIGHT = 600;

#pragma region functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
#pragma endregion


int main() {
	// glfw init: OpenGL 3.3
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	// Mac OS X
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// create window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tutorial04", NULL, NULL);
	if (!window) {
		printf("GLFW init Failed\n");
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("GLAD init Failed\n");
		return 0;
	}

	// load shader source code file
	unsigned int shaderProgram = loadShaders(
		"shaders/static.vert",
		"shaders/static.frag"
	);

	/* canvas's coordinate between [-1, 1]
	           y
			   ¡ü
			   | 1
			___|___
		-1	|  |  | 1
	   -----|--0--|----> x
			|__|__|
		       | -1
			   |
	*/
	// hexagen (6 points) between [-1, 1]
	static const float points[] = {
		0.0f,  0.5f, 0.0f, // 0
	   -0.4f,  0.2f, 0.0f, // 1
		0.4f,  0.2f, 0.0f, // 2
	   -0.4f, -0.2f, 0.0f, // 3
		0.4f, -0.2f, 0.0f, // 4
		0.0f, -0.5f, 0.0f, // 5
	};

	/*
	            0
			  /   \
           1/       \2
		   |         |
		  3|         |4
			\       /
			  \   /
			    5
	*/
	// 4 triangles = ¡ø012 + ¡ø345 + rectangle(1234)
	static const unsigned int triangles[] = {
		0, 1, 2,
		1, 2, 3,
		2, 3, 4,
		3, 4, 5,
	};

	// color data (length same as points when use EBO)
	static const float colors[] = {
		1.0f, 0.0f, 0.0f, // 0
		0.0f, 1.0f, 0.0f, // 1
		0.0f, 1.0f, 0.0f, // 2
		0.0f, 0.0f, 1.0f, // 3
		0.0f, 0.0f, 1.0f, // 4
		1.0f, 0.0f, 0.0f, // 5
	};

	// create VAO(vertex array object)
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// create EBO(element buffer object)
	unsigned int EBO = createBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles);

	// create VBO(vertex buffer object) --> vertex
	unsigned int vertexVBO = createBufferData(GL_ARRAY_BUFFER, points);
	// vertex attributes pointers (location 0) -> bind vertexVBO to vertex attribute
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader
		3,                  // attribute 0 is "vec3", so size = 3
		GL_FLOAT,           // attribute 0 vec3's element type is 'float'
		GL_FALSE,           // true means normalized, and in this case GL_FALSE means no need normalized.
		3 * sizeof(float),  // stride.
		(void*)0            // array buffer offset.
	);
	// enable vertex attribute localtion 0 
	glEnableVertexAttribArray(0);

	// create VBO(vertex buffer object) --> color
	unsigned int colorVBO = createBufferData(GL_ARRAY_BUFFER, colors);
	// vertex attributes pointers (location 1) -> bind colorVBO to vertex attribute
	glVertexAttribPointer(
		1,                  // attribute 1
		3,                  // attribute 1 is "vec3"
		GL_FLOAT,           // attribute 1 vec3's element type is 'float'
		GL_FALSE,           // normalized?
		3 * sizeof(float),  // stride
		(void*)0            // array buffer offset. 
	);
	// enable vertex attribute localtion 1
	glEnableVertexAttribArray(1);

	// unbind all VBOs, note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object
	// so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// ps: do not unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO but this rarely happens.
	// Modifying other VAOs requires a call to glBindVertexArray anyways. so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// Also, we can delete VBO when use glVertexAttribPointer
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &colorVBO);

	// wire frame mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ps: we can just bind once before 'render loop', cause just one VAO
	//glBindVertexArray(VAO);

	int count = sizeof(triangles) / sizeof(float); // count = 12 --> 4triangle = 12vertex
	// render loop
	while (!glfwWindowShouldClose(window)) {
		// input keycode
		processInput(window);
		
		// bg color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//draw cammad
		glUseProgram(shaderProgram);
		// seeing as we only have a single VAO there's no need to bind it every time.
		glBindVertexArray(VAO);
		// similar glDrawArrays
		glDrawElements(
			GL_TRIANGLES,       // specifies the mode
			count,              // the count or number of elements we'd like to draw. in this case is 4 triangles, so we can pass 4*3 = 12(vertices)
			GL_UNSIGNED_INT,    // triangle indices's type, in this case is 'static const unsigned int'
			0                   // allows us to specify an offset in the EBO
		);

		// swap buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// delete resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}
}