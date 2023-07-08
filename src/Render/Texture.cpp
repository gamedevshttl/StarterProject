#include "Texture.h"
#include <GLFW/glfw3.h>

Texture::Texture()
{
	glGenTextures(1, &id);
}

//void Texture::generate(GLuint width, GLuint height, GLuint imageFormat, unsigned char* data)
//{
//	glBindTexture(GL_TEXTURE_2D, id);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
//	glGenerateMipmap(GL_TEXTURE_2D);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	glBindTexture(GL_TEXTURE_2D, 0);
//}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, id);
}
