#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
using namespace glm;
using namespace std;
//------------------------------------------------
const static int SCR_WIDTH = 800, SCR_HEIGHT = 600;
static const float ROTATE_SPEED = 0.001f;

#pragma region functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int loadShaders();
inline void check_shader_status(GLuint shader);
inline void check_program_status(GLuint program);
float lerp(float a, float b, float f);
#pragma endregion

int main() {
	// glfw: init OpenGL 3.3
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	// Mac OSX
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	auto window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MyWindow", NULL, NULL);
	if (!window) {
		printf("glfw init failed\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
	
	// glad init
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("glad init failed\n");
		return -1;
	}
	
	// load shader source code
	unsigned int shaderProgram = loadShaders();

	/* pyramid
			     
	           0______________3
	          / \        /   /
			 /    \   /     / 
	        /      4       /
		   /    /   \     /
		  /   /      \   /
		1/_/__________\_/2
	*/
	static float vertices[] = {
		// position            // color
		-1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,  // 0
		-1.0f, -1.0f,  1.0f,   0.0f, 1.0f, 0.0f,  // 1
		 1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,  // 2
		 1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 0.0f,  // 3
		 0.0f,  1.0f,  0.0f,   0.0f, 1.0f, 1.0f,  // 4
	};

	// 6 triangles
	static const unsigned int triangles[] = {
		// bottom
		0, 1, 2,
		0, 2, 3,

		// left
		0, 1, 4,

		// right
		2, 3, 4,

		// front
		1, 2, 4,

		// back
		0, 4, 3,
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position vertex attribute (location 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color vertex attribute (location 1)
	glVertexAttribPointer(
		1,                          // attribute location = 1 
		3,                          // 'vec3' size is 3
		GL_FLOAT,                   // 'vec3''s element type is 'float'
		GL_FALSE,                   // normalized?
		6 * sizeof(float),          // stride. To get the next attribute value (e.g. the next x component of the position vector) in the data array we have to move 6 floats to the right, so a stride value = 6 * sizeof(float)
		(void*)(3 * sizeof(float))  // offset. The color attribute starts after 3 position data, so 3 * sizeof(float)
	);
	glEnableVertexAttribArray(1);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

	// unbind VBO EBO VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// delete VAO
	glDeleteBuffers(1, &VBO);

	// MVP
	static mat4 P = perspective(radians(60.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	static mat4 V = lookAt(vec3(4, 3, -3), vec3(0), vec3(0, 1, 0));
	static mat4 M = mat4(1.0f);

	// uniform
	unsigned int MVPLocation = glGetUniformLocation(shaderProgram, "MVP");
	unsigned int timeLocation = glGetUniformLocation(shaderProgram, "time");

	// Z-Buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	int count = sizeof(triangles) / sizeof(float); // count = 18 --> 5triangle = 18vertex
	while (!glfwWindowShouldClose(window)) {
		// bg color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw cammand
		float time = glfwGetTime();
		V = rotate(V, ROTATE_SPEED, vec3(0.0f, 1.0f, 0.0f)); // rotate by y axis
		auto MVP = P * V * M;

		glUseProgram(shaderProgram);
		glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &MVP[0][0]);
		glUniform4f(timeLocation, time / 4, time, 2 * time, abs(sin(time)));

		// Note that bind once VAO is also ok since just one VAO.
		glBindVertexArray(VAO);

		// EBO is likely to VAO. It just so happens that a VAO also keeps track of element buffer object bindings.
		// Note that bind once EBO is also ok, cause The glDrawElements function takes its indices from the EBO currently bound to the GL_ELEMENT_ARRAY_BUFFER target.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);

		// swap buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// delete resources
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shaderProgram);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS) 
		return;

	switch (key)
	{
		// on press keycode `ESC`
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
		default: break;
	}
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

unsigned int loadShaders()
{
	// vertex shader
	const char* vertexShaderSource =
		"#version 330 core\n"
		"layout (location = 0) in vec3 pos;\n"
		"layout (location = 1) in vec3 _color;\n"
		"uniform mat4 MVP;\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = MVP * vec4(pos, 1.0);\n"
		"   color = vec4(_color, 1.0);\n"
		"}\n";

	// fragment shader
	const char* fragmentShaderSource =
		"#version 330 core\n"
		"in vec4 color;\n"
		"uniform vec4 time;\n"
		"out vec4 c;\n"
		"void main()\n"
		"{\n"
		"   c = time.w * color;\n"
		"}\0";

	// compile shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(vertexShader);
	check_shader_status(vertexShader);
	glCompileShader(fragmentShader);
	check_shader_status(fragmentShader);

	// link shader program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	check_program_status(shaderProgram);

	// deatch && delete shader
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

// check compile shader
inline void check_shader_status(GLuint shader)
{
	int success;
	int length;
	std::vector<char> infoLog;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		int n = length + 1;
		infoLog.resize(n);
		glGetShaderInfoLog(shader, length, NULL, &infoLog[0]);
		printf("Error Compile Shader: %s\n", &infoLog[0]);
	}
}

// check link shader program
inline void check_program_status(GLuint program)
{
	int success;
	int length;
	std::vector<char> infoLog;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		int n = length + 1;
		infoLog.resize(n);
		glGetProgramInfoLog(program, length, NULL, &infoLog[0]);
		printf("Error Link Shader: %s\n", &infoLog[0]);
	}
}

float lerp(float a, float b, float t)
{
	return a + t * (b - a); // equivalent to  a*(1.0 - t) + b*t
}