#ifndef __MY_LIB_H__
#define __MY_LIB_H__

// tut01
GLFWwindow* createWindow(int width, int height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// tut02
unsigned int loadShaders(const char* vertex_file_path, const char* fragment_file_path);
// tut04
#include <my_lib.inl>
// tut05
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif // __MY_LIB_H__
