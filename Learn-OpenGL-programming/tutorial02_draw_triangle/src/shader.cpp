#include <glad/glad.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <conio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <shader.h>
using namespace std;

// load shader from file
GLuint loadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
    // Read Vertex Shader code from file
    string VertexShaderCode;
    std::ifstream ifs(vertex_file_path, std::ios::in);
    if (ifs.is_open()) {
        std::stringstream sstr;
        sstr << ifs.rdbuf();
        VertexShaderCode = sstr.str();
        ifs.close();
    }
    else {
        fmt::print("Impossible to open '{}'\nAre you in the right directory? Please check your working dir!\n", vertex_file_path);
        _getch();
        return 0;
    }

    // Read Fragment Shader code from file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }
    else {
        fmt::print("Impossible to open '{}'\nAre you in the right directory? Please check your working dir!\n", fragment_file_path);
        _getch();
        return 0;
    }

    // compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    char const* vertexShaderSource = VertexShaderCode.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check if compile vertex shader sucess
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        int infoLogLength;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        vector<char> infoLog(infoLogLength + 1);
        glGetShaderInfoLog(vertexShader, infoLogLength, NULL, &infoLog[0]);
        fmt::print("vertex shader failed to compile\n{}\n", infoLog); // #include <fmt/ranges.h>
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
        int infoLogLength;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        vector<char> infoLog(infoLogLength + 1);
        glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, &infoLog[0]);
        fmt::print("fragment shader failed to compile\n{}\n", infoLog); // #include <fmt/ranges.h>
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
        int infoLogLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> infoLog(infoLogLength + 1);
        glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, &infoLog[0]);
        fmt::print("shader program failed to link shader\n {}\n", infoLog); // #include <fmt/ranges.h>
    }
    // detach shader: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDetachShader.xhtml
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);

    // delete the shader objects once we've linked them into the program object;
    // cause we no longer need them anymore.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}