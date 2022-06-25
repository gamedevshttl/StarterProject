#ifndef SCENE_H
#define SCENE_H

#include "Node.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "glCore/gl_core_4_4.h"
#include <GLFW/glfw3.h>

class Scene : public Node
{
public:
	Scene(std::string_view tagScene);
	void init();
	void update(GLfloat dt);
	void draw();

private:
	void imGuiNewFrame();
	void inGuiRenderGUI();
private:
	Shader shader;
	Texture texture;
	GLuint VAO;

	float rotation;
};

#endif