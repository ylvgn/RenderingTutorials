#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>                  // glm::vec3
#include <glm/vec4.hpp>                  // glm::vec4
#include <glm/mat4x4.hpp>                // glm::mat4
#include <glm/ext/matrix_transform.hpp>  // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp>  // glm::pi

#include <my_lib.h>
#include <time.h>
#include <iostream>
using namespace std;
//-------------------------------------------------------------------
const int SCR_WIDTH = 800, SCR_HEIGHT = 600;
const float MOUSE_SENSITIVE = 0.025f;

bool is_pressing_mouse = false;
float cameraAngleX;
float cameraAngleY;
float mouseX;
float mouseY;

#pragma region functions
glm::mat4 camera(float Translate, glm::vec2 const& Rotate);
void cursor_position_callback(GLFWwindow* window, double x, double y);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
#pragma endregion


int main() {
	// create glfw window
	GLFWwindow* window = createWindow(SCR_WIDTH, SCR_HEIGHT);

	// https://www.glfw.org/docs/3.3/input_guide.html#input_mouse
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// load shader source code
	unsigned int shaderProgram = loadShaders(
		"shaders/static.vert",
		"shaders/static.frag"
	);

	// MVP matrix
	unsigned int MVPLocation = glGetUniformLocation(shaderProgram, "MVP");

	// cube
	// 8 points
	static const float points[] = {
		-1.0f, -1.0f, -1.0f, // 0
		 1.0f, -1.0f, -1.0f, // 1
		 1.0f, -1.0f,  1.0f, // 2
		-1.0f, -1.0f,  1.0f, // 3
		-1.0f,  1.0f, -1.0f, // 4
		 1.0f,  1.0f, -1.0f, // 5
		 1.0f,  1.0f,  1.0f, // 6
		-1.0f,  1.0f,  1.0f, // 7
	};

	/*    4________ 7
		  /       /|
		 /       / |
	   5/______6/  |
		|       |  |
		|       |  / 3
		|       | /
		|_______|/
		1       2
	*/

	// 12 triangles
	static const unsigned int triangles[] = {
		//bottom
		0, 1, 2,
		0, 2, 3,

		//top
		4, 5, 6,
		4, 6, 7,

		// left
		0, 4, 5,
		0, 1, 5,

		// right
		3, 2, 6,
		3, 6, 7,

		// back
		0, 4, 7,
		0, 3, 7,

		// front
		1, 2, 6,
		1, 5, 6,
	};

	static const int n = 36;      // 12 triangles * 3 points
	static const int len = n * 3; // 36 * 3 = 118(float)
	float vertices[len];
	for (int i = 0; i < n; i ++)
	{
		int index = triangles[i];
		for (int j = 0; j < 3; j ++) {
			int k = i * 3 + j;
			vertices[k] = points[index * 3 + j];
		}
	}

	float colors[len]; // random color [0~1]
	srand(time(0));
	for (int i = 0; i < n; i++)
	{
		int index         = 3 * i;
		float r           = (float)(rand() % 100) / 100.0f;
		float g           = (float)(rand() % 100) / 100.0f;
		float b           = (float)(rand() % 100) / 100.0f;
		colors[index]     = r;
		colors[index + 1] = g;
		colors[index + 2] = b;
	}

	// VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// VBO (vertex)
	unsigned int vertexVBO = createBufferData(GL_ARRAY_BUFFER, vertices);
	// vertex attributes pointers (location 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// VBO (color)
	unsigned int colorVBO = createBufferData(GL_ARRAY_BUFFER, colors);
	// vertex attributes pointers (location 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// unbind VAO VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// delete VBOs
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &colorVBO);

	// Z-buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// render loop
    while (!glfwWindowShouldClose(window))
    {
		glm::mat4 MVP = camera(5, glm::vec2(cameraAngleY, -cameraAngleX)); // 5m distance to canvas, view angle(x, y)

        // bg color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw command
		glUseProgram(shaderProgram);
		glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &MVP[0][0]);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36); // 12 triangles -> 36 vertexs

		// swap buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	// delete resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shaderProgram);
	glfwTerminate();
	return 0;
}

// example from: https://github.com/g-truc/glm
glm::mat4 camera(float Translate, glm::vec2 const& Rotate) {
	// project matrix(P)
	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);

	// view matrix(V)
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));

	// model matrix(M)
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

	return Projection * View * Model;
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
	if (!is_pressing_mouse) return;
	if (x == mouseX && y == mouseY) return;

	// on left-mouse-click pressing
	x *= MOUSE_SENSITIVE;
	y *= MOUSE_SENSITIVE;
	cameraAngleX += (y - mouseY);
	cameraAngleY += (x - mouseX);
	mouseX = x;
	mouseY = y;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	// on left-mouse down
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) is_pressing_mouse = true;

	// on left-mouse up
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) is_pressing_mouse = false;

	/* or
		int left_mouse_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

		// on left-mouse down
		if (left_mouse_state == GLFW_PRESS) {
			is_pressing_mouse = true;
		}
		// on left-mouse up
		if (left_mouse_state == GLFW_RELEASE) {
			is_pressing_mouse = false;
		}
	*/
}