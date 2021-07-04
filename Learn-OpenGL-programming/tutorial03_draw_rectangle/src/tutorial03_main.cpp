#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
using namespace std;
//-----------------------------------
const int SCR_WIDTH = 800, SCR_HEIGHT = 600;

#pragma region functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::vector<float> getRectangleVertices();
unsigned int loadShaders();
inline void check_shader_status(GLuint shader);
inline void check_program_status(GLuint program);
#pragma endregion

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    // Mac OS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tutorial03", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return 0;
    }

    // create shader program
    unsigned int shaderProgram = loadShaders();

    // create rectangle vertices (user_data)
    vector<float> rectangle_vertices = getRectangleVertices();
    int n = (int)rectangle_vertices.size();   // n = 18 --> 2triangle = 6vertex = 18 float
    float* vertices = &rectangle_vertices[0]; // vector<float> convert to float*

    // create VAO(vertex array object)
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // create VBO(vertex buffer objects)
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    // bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // copy user data to VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * n, vertices, GL_STATIC_DRAW);

    // vertex attributes pointers (location 0)
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader
        3,                  // attribute 0 is "vec3", so size = 3
        GL_FLOAT,           // attribute 0 vec3's element type is 'float', opengl typedef it 'GL_FLOAT'
        GL_FALSE,           // true means normalized, and in this case GL_FALSE means no need normalized.
        3 * sizeof(float),  // stride. we could've also specified the stride as 0 to let OpenGL determine the stride (this only works when values are tightly packed)
        (void*)0            // array buffer offset. (where the position data begins in the buffer)
    );
    // enable vertex attribute localtion 0
    glEnableVertexAttribArray(0);

    // unbind VAO VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
	// render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // bg color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);

        // draw command
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6); // 2 triangles -> 6 vertex

        // swap buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, true);
    }
}

std::vector<float> getRectangleVertices()
{
    // rectangle points (4 points)
    static const float points[] = {
        -0.5f, -0.5f, 0.0f, // 0
        -0.5f,  0.5f, 0.0f, // 1
         0.5f, -0.5f, 0.0f, // 2
         0.5f,  0.5f, 0.0f, // 3
    };

    /*
        0_______1
        |      /|
        |    /  |
        |  /    |
        |/______|
        2      3
    */

    // 2 triangles
    static const unsigned int triangles[] = {
        0, 1, 2,
        1, 2, 3,
    };

    int len = sizeof(triangles) / sizeof(int);
    int n = len * 3;
    std::vector<float> res(n); // 2 triangle = 6 point (xyz) = 18 float
    for (int i = 0; i < len; i++)
    {
        auto index = triangles[i];
        for (int j = 0; j < 3; j++) {
            int k = i * 3 + j;
            res[k] = points[index * 3 + j];
        }
    }
    return res;
}

// loader shader source code from string
unsigned int loadShaders()
{
    const char* vertexShaderSource = 
        "#version 330 core\n"
        "layout(location = 0) in vec3 pos;\n"
        "void main()\n"
        "{\n"
        "gl_Position = vec4(pos, 1.0);\n"
        "}\n";

    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "color = vec4(1, 1, 0, 1);\n"
        "}\n";

    // create shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    // compile vertex shader
    glShaderSource(
        vertexShader,        // shader object
        1,                   // specifies how many strings we're passing as source code. and in this case'vertexShaderSource' is 1 string.
        &vertexShaderSource, // shader source code(string)
        NULL
    );
    glCompileShader(vertexShader);

    // check vertex shader
    check_shader_status(vertexShader);

    // compile fragment shader
    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        NULL
    );
    glCompileShader(fragmentShader);

    // check fragment shader
    check_shader_status(fragmentShader);
    
    // create shader program
    GLuint shaderProgram = glCreateProgram();
    // attach shader
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // link shader program
    glLinkProgram(shaderProgram);

    // check shader program
    check_program_status(shaderProgram);

    // detach shader
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    // delete shader
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

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