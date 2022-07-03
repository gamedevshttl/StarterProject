#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:
	glm::mat4 getViewMatrix();
	void init(GLfloat initlastX, GLfloat initlastY);
	void processKeyboard(int key, int action);
	void processMouseMovement(double xpos, double ypos);
	void updateCameraVectors(GLfloat dt);

	glm::vec3 position{ glm::vec3(0.0f, 0.0f, 3.0f) };
	glm::vec3 front{ glm::vec3(0.0f, 0.0f, -1.0f) };
	glm::vec3 up{ glm::vec3(0.0f, 1.0f, 0.0f) };
	GLfloat speed{ 0.5f };

	bool pressed[GLFW_KEY_LAST] = {};
	GLfloat lastX, lastY;
	GLfloat yaw{ -90.0f };
	GLfloat pitch{ 0.0f };
	GLfloat xoffset{ 0.f };
	GLfloat yoffset{ 0.f };
	GLfloat sensitivity{ 10.f };
	bool firstMouse{ false };
};

#endif