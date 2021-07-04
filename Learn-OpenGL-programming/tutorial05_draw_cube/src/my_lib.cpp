#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <my_lib.h>

GLFWwindow* createWindow(int width, int height) {
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
    // --------------------
    GLFWwindow* window = glfwCreateWindow(width, height, "MyWindow", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return 0;
    }
    return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// input keyboard
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;
    switch (key)
    {
        // keycode 'ESC'
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        default: break;
    }
}

// load shader from file (path relative to 'Debugging Working Directory' )
GLuint loadShaders(const char* vertex_file_path, const char* fragment_file_path) {
    std::stringstream sstr;
    std::string vertexShaderSourceCode;
    std::string fragmentShaderSourceCode;

    // read vertex shader code from file
    std::ifstream ifs(vertex_file_path, std::ifstream::in);
    if (!ifs.is_open()) {
        printf("Impossible to open '%s'\nAre you in the right directory? Please check your working directory!\n", vertex_file_path);
        return 0;
    }
    sstr << ifs.rdbuf();
    vertexShaderSourceCode = sstr.str();
    ifs.close();
    sstr.str("");

    // read fragment shader code from file
    ifs.open(fragment_file_path, std::ifstream::in);
    if (!ifs.is_open()) {
        printf("Impossible to open '%s'\nAre you in the right directory? Please check your working directory!\n", fragment_file_path);
        return 0;
    }
    sstr << ifs.rdbuf();
    fragmentShaderSourceCode = sstr.str();
    ifs.close();
    sstr.str("");

    // ----------------------------------------------------------------
    int infoLogLength;
    int success;
    int n;
    std::vector<char> infoLog;

    // compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    char const* vertexShaderSource = vertexShaderSourceCode.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check if compile vertex shader sucess
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        n = infoLogLength + 1;
        infoLog.resize(n);
        glGetShaderInfoLog(vertexShader, infoLogLength, NULL, &infoLog[0]);
        printf("vertex shader failed to compile\n%s\n", &infoLog[0]);
    }

    // compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char const* fragmentShaderSource = fragmentShaderSourceCode.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check if compile fragment shader sucess
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        n = infoLogLength + 1;
        infoLog.resize(n);
        glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, &infoLog[0]);
        printf("fragment shader failed to compile\n%s\n", &infoLog[0]);
    }

    // create shaders program
    unsigned int shaderProgram = glCreateProgram();
    // attach vertex shader && fragment shader
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // link shader program
    glLinkProgram(shaderProgram);
    // check if linking a shader program sucess
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        n = infoLogLength + 1;
        infoLog.resize(n);
        glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, &infoLog[0]);
        printf("shader program failed to link shader\n%s\n", &infoLog[0]);
    }
    // detach shader
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);

    // delete the shader objects once we've linked them into the program object;
    // cause we no longer need them anymore.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}