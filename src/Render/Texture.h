#ifndef TEXTURE_H
#define TEXTURE_H

#include "glCore/gl_core_4_4.h"

class Texture
{
public:
	Texture();
	void generate(GLuint aWidth, GLuint aHeight, unsigned char* data);
	void bind() const;

	GLuint id;
	GLuint width = 0;
	GLuint height = 0;
	GLuint internalFormat = GL_RGB;
	GLuint imageFormat = GL_RGB;

	GLuint wrapT = GL_REPEAT;
	GLuint wrapS = GL_REPEAT;
	GLuint filterMin = GL_LINEAR;
	GLuint filterMax = GL_LINEAR;

};

#endif