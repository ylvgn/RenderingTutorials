#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <shader.h>
#include <iostream>
using namespace std;
//-------------------------------------------------------------------
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

#pragma region functions
GLFWwindow* createWindow();
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int load_shaders();
#pragma endregion


int main()
{
    auto window = createWindow();
    if (!window) {
        fmt::print("Failed to initialize window\n");
        return 0;
    }
    
    // compile and link vertex shader and fragment shader
    unsigned int shaderProgram = load_shaders();
    // or uncomment this call to load shader from file
    //auto shaderProgram = loadShaders("shaders/static.vert", "shaders/static.frag");

    // user-data(VBO)
    float vertices[] =
    {
        -0.5f, -0.5f, 0.0f, // 0
         0.5f, -0.7f, 0.0f, // 1
         0.0f,  0.5f, 0.0f, // 2
    };

    // create buffer
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    // then bind VBO buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // copy buffer data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // enable vertex attribute localtion 0
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound VBO
    // so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards
    // so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways
    // so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons mode.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // keycode input
        processInput(window);

        // bg color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw command
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3); // 0 start index, count = 3 vertex
        // glBindVertexArray(0);          // ps: no need to unbind it every time 

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    
    // glfw terminate
    glfwTerminate();
    return 0;
}


GLFWwindow* createWindow() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tutorial02", NULL, NULL);
    if (window == NULL)
    {
        fmt::print("Failed to create GLFW window\n");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fmt::print("Failed to initialize GLAD\n");
        return 0;
    }
    return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    // on press Keycode.ESC
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

//load shader code from string
unsigned int load_shaders() {
    static const int LOG_LENGTH = 512;
    int  success;
    char infoLog[LOG_LENGTH];

    // vertex shader source code
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    // fragment shader source code
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

    // create vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // The glShaderSource function takes the shader object to compile to as its first argument.
    // The second argument specifies how many strings we're passing as source code, which is only one.
    // The third parameter is the actual source code of the vertex shader
    // and we can leave the 4th parameter to NULL.
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check if compile shader sucess
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, LOG_LENGTH, NULL, infoLog);
        fmt::print("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n {}\n", infoLog);
    }

    // create fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, LOG_LENGTH, NULL, infoLog);
        fmt::print("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n {} \n", infoLog);
    }

    // link shaders
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, LOG_LENGTH, NULL, infoLog);
        fmt::print("ERROR::SHADER::PROGRAM::LINKING_FAILED\n {}\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}