#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "Render/Shader.h"
#include "Render/Texture.h"
#include <map>

class ResourceManager
{
public:
	ResourceManager() = delete;

	static Shader loadShader(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile, std::string_view name);
	static Shader getShader(std::string_view name);

	static Texture loadTexture(const GLchar *textureFile, GLboolean alpha, std::string_view name);
	static Texture getTexture(std::string_view name);

	static void clear();	

	static std::map<std::string, Shader> shaders;
	static std::map<std::string, Texture> textures;
private:
	static Shader loadShaderFromFile(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile);
	static Texture loadTextureFromFile(const GLchar *textureFile, GLboolean alpha);
};

#endif