#ifndef SCENELIGHT_H
#define SCENELIGHT_H

#include "Node.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "Camera/Camera.h"
#include "glCore/gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <memory>
#include "Object/objmesh.h"

struct GLFWwindow;
class Torus;
class Teapot;
class ObjMesh;

struct lightPosition
{
	lightPosition(float aX, float aZ) : x(aX), z(aZ)
	{}

	float x;
	float z;
};

class SceneLight : public Node
{
public:
	SceneLight(std::string_view tagScene);
	void init(int screenWidth, int screenHeight);
	void keyboardCallback(int key, int scancode, int action, int mode);
	void mouseCallback(double xpos, double ypos);
	void update(GLfloat dt);
	void draw();

private:
	void imGuiNewFrame();
	void inGuiRenderGUI();
private:
	int screenWidth;
	int screenHeight;

	Shader shader;
	Texture texture;
	GLuint VAO;


	float rotation{ 0.f };
	glm::mat4 rotationMatrix;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	GLuint lampVAO;
	glm::mat4 lampModel;

	Camera camera;

	std::shared_ptr<Torus> pTorus;
	std::shared_ptr<Teapot> pTeapot;
	std::unique_ptr<ObjMesh> pMesh;
	std::vector<lightPosition> lightPosVector;

	float angle = 0.0;
};

#endif