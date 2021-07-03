#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <my_lib.h>

// load shader from file
GLuint loadShaders(const char* vertex_file_path, const char* fragment_file_path) {
    // read vertex shader code from file
    std::string VertexShaderCode;
    std::ifstream ifs(vertex_file_path, std::ios::in);
    if (ifs.is_open()) {
        std::stringstream sstr;
        sstr << ifs.rdbuf();
        VertexShaderCode = sstr.str();
        ifs.close();
    } else {
        printf("Impossible to open '%s'\nAre you in the right directory? Please check your working dir!\n", vertex_file_path);
        return 0;
    }

    // read fragment shader code from file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    } else {
        printf("Impossible to open '%s'\nAre you in the right directory? Please check your working dir!\n", fragment_file_path);
        return 0;
    }

    int infoLogLength;
    int success;
    int n;

    // compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    char const* vertexShaderSource = VertexShaderCode.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check if compile vertex shader sucess
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        n = infoLogLength + 1;
        std::vector<char> infoLog(n);
        glGetShaderInfoLog(vertexShader, infoLogLength, NULL, &infoLog[0]);
        printf("vertex shader failed to compile\n%s\n", &infoLog[0]);
    }

    // compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char const* fragmentShaderSource = FragmentShaderCode.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check if compile fragment shader sucess
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        n = infoLogLength + 1;
        std::vector<char> infoLog(n);
        glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, &infoLog[0]);
        printf("fragment shader failed to compile\n%s\n", &infoLog[0]);
    }

    // create shaders program
    unsigned int shaderProgram = glCreateProgram();
    // link(attach) shader
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check if linking a shader program sucess
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        n = infoLogLength + 1;
        std::vector<char> infoLog(n);
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