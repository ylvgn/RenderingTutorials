#ifndef __SHADER_H__
#define __SHADER_H__

#include <glad/glad.h>

#define LOG_MAX_LENGTH 1024

// tut02
class Shader
{
public:
	GLuint shader_program;
	Shader(const char* vertex_shader_path, const char* fragment_shader_path);
	void use_program();
	void Shader::set_int(const char* name, int value) const;
	~Shader();
};
#endif // !__SHADER_H__