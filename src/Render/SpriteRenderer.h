#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "glCore/gl_core_4_4.h"
#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class SpriteRenderer
{
public:
	SpriteRenderer(Shader& aShader);
	~SpriteRenderer();

	void drawSprite(Texture& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
private:
	Shader shader;
	GLuint VAO;
	void initRenderData();
};

#endif

