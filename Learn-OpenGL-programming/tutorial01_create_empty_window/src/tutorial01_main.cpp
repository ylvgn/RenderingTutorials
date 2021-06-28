#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;
//---------------------------------------------------------------
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

#pragma region functions
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
#pragma endregion

int main()
{
	// glfw: initialize and configure
	glfwInit();
	// use OpenGL version 3.3+
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // specified OpenGL core profile must support our configure (in this case 3.3)

#ifdef __APPLE__
	// Mac OS X configure
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// create glfw window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tutorial01", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate(); // glfw: terminate, clearing all previously allocated GLFW resources.
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback); // register callback for resize window

	// glad: load all OpenGL function pointers
	// Pointer address source form glfw based on which OS we're compiling for
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to init GLAD" << endl;
		return -1;
	}

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);

		// rendering custom commands
		glClearColor(1, 1, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT); // framebuffer (color buffer | depth buffer | stencil buffer )

		// glfw: swap buffers
		glfwSwapBuffers(window);

		// glfw: poll IO events (keys pressed/released, mouse moved etc.)
		glfwPollEvents();
	}

	glfwTerminate(); // glfw: terminate, clearing all previously allocated GLFW resources.
	return 0;
}

// glfw: register custom callback
// typedef void (* GLFWframebuffersizefun)(GLFWwindow*,int,int);
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// process all input (listening keys pressed)
void processInput(GLFWwindow* window)
{
	// on press Keycode.ESC
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}