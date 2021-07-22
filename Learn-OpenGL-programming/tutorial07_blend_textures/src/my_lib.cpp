#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <my_lib.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS) {
		return;
	}

	switch (key)
	{
		case GLFW_KEY_ESCAPE: // on press keycode `ESC`
			glfwSetWindowShouldClose(window, true);
			break;
		default:
			break;
	}
}

void error_callback(int error_code, const char* description)
{
	printf("GLFW: error_code:%d\ndescription:%s\n", error_code, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}