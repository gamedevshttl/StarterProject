#ifndef SHADER_H
#define SHADER_H

#include "glCore/gl_core_4_4.h"
#include <string_view>
#include <glm/glm.hpp>

class Shader
{
public:
	Shader();
	
	Shader& use();
	void compile(const GLchar *vertex_source, const GLchar *fragment_source);
	GLuint getId() { return id; }

	void setFloat(const GLchar* name, GLfloat value, GLboolean useShader = false);
	void setInt(const GLchar* name, GLint value, GLboolean useShader = false);
	void setVector2f(const GLchar* name, const glm::vec2& value, GLboolean useShader = false);
	void setVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
	void setVector3f(const GLchar* name, const glm::vec3& value, GLboolean useShader = false);
	void setVector4f(const GLchar* name, const glm::vec4& value, GLboolean useShader = false);
	void setMatrix3(const GLchar* name, const glm::mat3& matrix, GLboolean useShader = false);
	void setMatrix4(const GLchar *name, const glm::mat4& matrix, GLboolean useShader = false);
	void setSubroutine(GLenum shaderType, const GLchar* name, GLboolean useShader = false);
private:
	void checkCompileError(GLuint object, std::string_view type);
	GLuint id;
};

#endif