#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <my_lib.h>
#include <shader.h>
using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>   // Loading and creating textures

//-------------------------------
static void draw_scene(GLFWwindow* window, double t);
//-------------------------------
const static int SCR_WIDTH = 800, SCR_HEIGHT = 600;


const static float vertices[] = {
	// positions          // colors(rgb yellow)   // texture coords(UV)
	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,       1.0f, 1.0f,   // top right
	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,       1.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,       0.0f, 0.0f,   // bottom left
	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,       0.0f, 1.0f,   // top left 
};

// rectangle (2 triangles)
const static unsigned int indices[] = {
	0, 1, 3,
	1, 2, 3,
};

#include <stdio.h>

int main() {
	// glfw init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	// Mac OS
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MyWindow", 0, 0);
	if (!window) {
		printf("glfw init failed\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetErrorCallback(error_callback);

	// glad init
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("glad init failed\n");
		glfwTerminate();
		return -1;
	}

	// load shader
	Shader m_shader = Shader("shaders/static.vert", "shaders/static.frag");

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// don't forget to activate/use the shader before setting uniforms!
	m_shader.use_program();
	m_shader.set_int("Tex1", 0);
	m_shader.set_int("Tex2", 1);

	// create VAO VBO EBO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// vertex attribute location 0 -> position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// vertex attribute location 1 -> color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// vertex attribute  location 2 -> texture coord(UV)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	/* textures workflow:
		gen tex (glGenTextures)
		-> bind tex (glBindTexture)
		-> config tex (glTexParameterxxx)
		-> load tex (stbi_load)
		-> assign textures data (glTexImage2D)
		-> gen mipmap (glGenerateMipmap)
	*/
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set texture wrapping parameters (s t r -> x y z)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // MIN == minifying
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // MAG == magnifying

	// load image, create texture and generate mipmaps
	int tex_width, tex_height;
	int nrChannels; // number of color channels
	unsigned char* data = stbi_load("wall.jpg", &tex_width, &tex_height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(
			GL_TEXTURE_2D,    // generate a texture on the currently bound texture2D object
			0,                // The second argument specifies the 'mipmap level' for which we want to create a texture for if you want to set each mipmap level manually, but we'll leave it at the base level which is 0
			GL_RGB,           // 3rd argument tells OpenGL in what kind of format we want to store the texture. Our image has only RGB values so we'll store the texture with RGB values as well.
			tex_width,
			tex_height,
			0,                // always be 0 (some legacy stuff).
			GL_RGB,           // The 7th and 8th argument specify the format and datatype of the source image.
			GL_UNSIGNED_BYTE, // We loaded the image with RGB values and stored them as chars(bytes) so we'll pass in the corresponding values
			data              // the actual image data.
		);
		// if we want to use mipmaps we have to specify all the different images manually (by continually incrementing the second argument)
		// `second argument`=0 means only has the base-level of the texture image loaded.
		// We could call glGenerateMipmap after generating the texture.
		// This will automatically generate all the required mipmaps for the currently bound texture.
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		printf("Failed to load texture1\n");
	}

	// After we're done generating the texture and its corresponding mipmaps, it is good practice to free the image memory:
	stbi_image_free(data);

	unsigned int texture2;
	glGenTextures(1, &texture2);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("awesomeface.png", &tex_width, &tex_height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RG
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		printf("Failed to load texture2\n");
	}
	stbi_image_free(data);

	// unbine VAO VBO EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// delete VBO
	glDeleteBuffers(1, &VBO);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// bg color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_shader.use_program();
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(
			GL_TRIANGLES,
			6,  // the count or number of elements we'd like to draw. in this case is 2 triangles, so we can pass 2*3 = 6(vertices) 
			GL_UNSIGNED_INT,
			0   // offset in EBO
		);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// delete resources
	m_shader.~Shader();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}