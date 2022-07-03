#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::init(GLfloat initlastX, GLfloat initlastY)
{
	lastX = initlastX;
	lastY = initlastY;
}

void Camera::processKeyboard(int key, int action)
{
	if (action == GLFW_PRESS)
		pressed[key] = true;
	else if (action == GLFW_RELEASE)
		pressed[key] = false;
}

void Camera::processMouseMovement(double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	xoffset = xpos - lastX;
	yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
}

void Camera::updateCameraVectors(GLfloat dt)
{
	xoffset *= sensitivity * dt;
	yoffset *= sensitivity * dt;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 localFront;
	localFront.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	localFront.y = sin(glm::radians(pitch));
	localFront.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front = glm::normalize(localFront);

	if (pressed[GLFW_KEY_W])
		position += speed * front * dt;
	if (pressed[GLFW_KEY_S])
		position -= speed * front * dt;
	if (pressed[GLFW_KEY_A])
		position -= glm::normalize(glm::cross(front, up)) * speed * dt;
	if (pressed[GLFW_KEY_D])
		position += glm::normalize(glm::cross(front, up)) * speed * dt;
}