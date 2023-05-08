#include <iostream>
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

Shader::Shader()
{
}

Shader& Shader::use()
{
	glUseProgram(id);
	return *this;
}

void Shader::compile(const GLchar *vertexSource, const GLchar *fragmentSource)
{
	GLuint vertex, fragment;
	
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSource, nullptr);
	glCompileShader(vertex);
	checkCompileError(vertex, "vertex");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSource, nullptr);
	glCompileShader(fragment);
	checkCompileError(fragment, "fragment");

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);
	checkCompileError(id, "programm");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}


void Shader::checkCompileError(GLuint object, std::string_view type)
{
	GLint succes;
	GLchar infoLog[1024];

	if (type == "programm")
	{
		glGetProgramiv(object, GL_LINK_STATUS, &succes);
		if (!succes)
		{
			glGetProgramInfoLog(object, 1024, nullptr, infoLog);
			std::cout << type << " : " << infoLog << std::endl;
		}
	}
	else
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &succes);
		if (!succes)
		{
			glGetShaderInfoLog(object, 1024, nullptr, infoLog);
			std::cout << type << " : " << infoLog << std::endl;
		}
	}
}

void Shader::setFloat(const GLchar* name, GLfloat value, GLboolean useShader)
{
	if (useShader)
		use();
	glUniform1f(glGetUniformLocation(id, name), value);

}

void Shader::setInt(const GLchar* name, GLint value, GLboolean useShader)
{
	if (useShader)
		use();
	glUniform1i(glGetUniformLocation(id, name), value);
}

void Shader::setVector2f(const GLchar* name, const glm::vec2& value, GLboolean useShader)
{
	if (useShader)
		use();
	glUniform2f(glGetUniformLocation(id, name), value.x, value.y);
}

void Shader::setVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
{
	if (useShader)
		use();
	glUniform3f(glGetUniformLocation(id, name), x, y, z);
}

void Shader::setVector3f(const GLchar* name, const glm::vec3& value, GLboolean useShader)
{
	if (useShader)
		use();
	glUniform3f(glGetUniformLocation(id, name), value.x, value.y, value.z);
}

void Shader::setVector4f(const GLchar* name, const glm::vec4& value, GLboolean useShader)
{
	if (useShader)
		use();
	glUniform4f(glGetUniformLocation(id, name), value.x, value.y, value.z, value.w);
}

void Shader::setMatrix4(const GLchar *name, const glm::mat4& matrix, GLboolean useShader)
{
	if (useShader)
		use();
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setSubroutine(GLenum shaderType, const GLchar* name, GLboolean useShader)
{
	if (useShader)
		use();
	GLuint diffuse = glGetSubroutineIndex(id, shaderType, name);
	glUniformSubroutinesuiv(shaderType, 1, &diffuse);
}