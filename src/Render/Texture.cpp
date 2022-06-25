#include "Texture.h"
#include <GLFW/glfw3.h>

Texture::Texture()
{
	glGenTextures(1, &id);
}

void Texture::generate(GLuint aWidth, GLuint aHeight, unsigned char* data)
{
	width = aWidth;
	height = aHeight;

	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax);

	glBindTexture(GL_TEXTURE_2D, 0);

}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, id);
}
