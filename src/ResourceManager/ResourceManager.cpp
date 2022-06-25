#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::map<std::string, Shader> ResourceManager::shaders;
std::map<std::string, Texture> ResourceManager::textures;

Shader ResourceManager::loadShader(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile, std::string_view name)
{
	shaders[name.data()] = loadShaderFromFile(vertexShaderFile, fragmentShaderFile);
	return shaders[name.data()];
}

Shader ResourceManager::getShader(std::string_view name)
{
	return shaders[name.data()];
}

Texture ResourceManager::loadTexture(const GLchar *textureFile, GLboolean alpha, std::string_view name)
{
	textures[name.data()] = loadTextureFromFile(textureFile, alpha);
	return textures[name.data()];
}

Texture ResourceManager::getTexture(std::string_view name)
{
	return textures[name.data()];
}

void ResourceManager::clear()
{
	for (auto iter : shaders) {
		glDeleteProgram(iter.second.getId());
	}

	for (auto iter : textures) {
		glDeleteTextures(1, &iter.second.id);
	}

	shaders.clear();
	textures.clear();
}

Shader ResourceManager::loadShaderFromFile(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile)
{
	std::string vertexCode;
	std::string fragmentCode;

	std::ifstream vertexShader(vertexShaderFile);
	std::ifstream fragmentShader(fragmentShaderFile);

	std::stringstream vertexShaderStream, fragmentShaderStream;

	vertexShaderStream << vertexShader.rdbuf();
	fragmentShaderStream << fragmentShader.rdbuf();

	vertexShader.close();
	fragmentShader.close();

	vertexCode = vertexShaderStream.str();
	fragmentCode = fragmentShaderStream.str();

	Shader shader;
	shader.compile(vertexCode.c_str(), fragmentCode.c_str());
	return shader;
}

Texture ResourceManager::loadTextureFromFile(const GLchar *file, GLboolean alpha)
{
	Texture texture;
	if (alpha) {
		texture.internalFormat = GL_RGBA;
		texture.imageFormat = GL_RGBA;
	}

	int width, height, nr_channels;

	unsigned char* image = stbi_load(file, &width, &height, &nr_channels, 0);

	if (image) {
		GLenum format;
		if (nr_channels == 1)
			texture.imageFormat = GL_RED;
		else if (nr_channels == 3)
			texture.imageFormat = GL_RGB;
		else if (nr_channels == 4)
			texture.imageFormat = GL_RGBA;

		texture.generate(width, height, image);
		stbi_image_free(image);
	}
	else {
		std::cout << "Failed load texture " << file << std::endl;
	}

	return texture;
}
