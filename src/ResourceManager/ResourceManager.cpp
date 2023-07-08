#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <nlohmann/json.hpp>

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

unsigned char* loadPixels(const std::string& fName, int& width, int& height) {
	int bytesPerPix;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(fName.c_str(), &width, &height, &bytesPerPix, 4);
	return data;
}

Texture ResourceManager::loadTexture(const GLchar *textureFile, GLboolean alpha, std::string_view name)
{
	//textures[name.data()] = loadTextureFromFile(textureFile, alpha);
	//return textures[name.data()];

	Texture texture;

	int width, height;
	unsigned char* data = loadPixels(textureFile, width, height);

	if (data != nullptr) {
		GLuint tex;
		glGenTextures(1, &texture.id);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		stbi_image_free(data);
		textures[name.data()] = texture;
	}
	return texture;
}


Texture ResourceManager::loadCubeMap(const GLchar* textureFile, std::string_view name)
{
	Texture texture;

	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);

	const char* suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
	GLuint targets[] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	stbi_set_flip_vertically_on_load(true);
	GLint w, h;

	const std::string& baseName = textureFile;
	// Load the first one to get width/height
	std::string texName = baseName + "_" + suffixes[0] + ".png";
	GLubyte* data = loadPixels(texName.c_str(), w, h);

	// Allocate immutable storage for the whole cube map texture
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, w, h);

	glTexSubImage2D(targets[0], 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	// Load the other 5 cube-map faces
	for (int i = 1; i < 6; i++) {
		std::string texName = baseName + "_" + suffixes[i] + ".png";
		GLubyte* data = loadPixels(texName.c_str(), w, h);
		glTexSubImage2D(targets[i], 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	textures[name.data()] = texture;

	return texture;
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

std::vector<GLfloat> ResourceManager::loadData(const GLchar* dataFile)
{
	std::ifstream file(dataFile);
	std::stringstream fileStream;
	fileStream << file.rdbuf();
	file.close();

	nlohmann::json myjson = nlohmann::json::parse(fileStream.str());
	auto& data = myjson["data"];

	std::vector<GLfloat> dataVector;
	for (auto it = data.begin(); it != data.end(); ++it)
		dataVector.emplace_back(it->get<float>());

	return dataVector;
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
	GLuint imageFormat;
	if (alpha) {
		//texture.internalFormat = GL_RGBA;
		//texture.imageFormat = GL_RGBA;
		imageFormat = GL_RGBA;
	}

	int width, height, nr_channels;

	unsigned char* image = stbi_load(file, &width, &height, &nr_channels, 0);

	if (image) {
		GLenum format;
		if (nr_channels == 1)
			imageFormat = GL_RED;
		else if (nr_channels == 3)
			imageFormat = GL_RGB;
		else if (nr_channels == 4)
			imageFormat = GL_RGBA;

		//texture.generate(width, height, imageFormat, image);
		stbi_image_free(image);
	}
	else {
		std::cout << "Failed load texture " << file << std::endl;
	}

	return texture;
}
