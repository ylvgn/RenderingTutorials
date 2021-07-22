#include <shader.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <iostream>

/*---------------------------------------------------------------*/
// tut03
inline void check_shader_status(GLuint shader, const char* msg);
inline void check_program_status(GLuint program);
// tut07
inline std::string readAllText(const char* file_path);
/*---------------------------------------------------------------*/
char info_log[LOG_MAX_LENGTH];


Shader::Shader(const char* vertex_file_path, const char* fragment_file_path)
    : shader_program(0u)
{
    std::string vertex_source_code = readAllText(vertex_file_path);
    std::string fragment_source_code = readAllText(fragment_file_path);
    char const* vertex_shader_source = vertex_source_code.c_str();
    char const* fragment_shader_source = fragment_source_code.c_str();

    // compile shader source code
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    check_shader_status(vertex_shader, "vertex shader");

    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    check_shader_status(fragment_shader, "fragment shader");

    // create shader program
    this->shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    check_program_status(shader_program);

    // delete shader source
    glDetachShader(shader_program, vertex_shader);
    glDetachShader(shader_program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

Shader::~Shader()
{
    if (this->shader_program) {
        glDeleteProgram(this->shader_program);
    }
}

void Shader::use_program()
{
    if (!shader_program) {
        return;
    }
    glUseProgram(this->shader_program);
}

void Shader::set_int(const char* name, int value) const
{
    GLint location = glGetUniformLocation(shader_program, name);
    glUniform1i(location, value);
}

// --------------------------------------------------------------
inline std::string readAllText(const char* file_path)
{
    std::ifstream ifs;
    std::stringstream sstr;

    // ensure ifstream objects can throw exceptions:
    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        ifs.open(file_path, std::ifstream::in);
        if (ifs.is_open())
        {
            sstr << ifs.rdbuf();
            ifs.close();
            return sstr.str();
        }
        else {
            printf("Impossible to open '%s'\nAre you in the right directory?\nPlease check your working directory!\n", file_path);
        }
    }
    catch (std::ifstream::failure e)
    {
        printf("Read file failed: file_path = %s ", file_path);
    }
}

// check compile shader
inline void check_shader_status(GLuint shader, const char* msg)
{
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, LOG_MAX_LENGTH, NULL, info_log);
        printf("[%s] error compile shader: %s\n", msg, info_log);
        glDeleteShader(shader);
        return;
    }
}

// check link shader program
inline void check_program_status(GLuint program)
{
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, LOG_MAX_LENGTH, NULL, info_log);
        printf("error link shader program: %s\n", info_log);
        glDeleteProgram(program);
        return;
    }
}
