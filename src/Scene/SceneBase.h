#ifndef SCENEBASE_H
#define SCENEBASE_H

#include "Node.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "Camera/Camera.h"
#include "glCore/gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <memory>

//struct GLFWwindow;
//class Torus;
//class Teapot;

struct lightPosition
{
	lightPosition(float aX, float aZ) : x(aX), z(aZ)
	{}

	float x;
	float z;
};

class SceneBase : public Node
{
public:
	SceneBase(std::string_view tagScene);
	virtual void init(int screenWidth, int screenHeight) = 0;
	virtual void keyboardCallback(int key, int scancode, int action, int mode) = 0;
	virtual void mouseCallback(double xpos, double ypos) = 0;
	virtual void update(GLfloat dt) = 0;
	virtual void draw() = 0;
};

#endif