#include "Application.h"
#include "Scene/Scene.h"
#include "Scene/SceneLight.h"
#include "Scene/SceneTexture.h"
#include "Scene/SceneFBO.h"

#include "imgui.h"
#include "Dependencies/imgui_impl_glfw.h"
#include "Dependencies/imgui_impl_opengl3.h"

#include "glCore/gl_core_4_4.h"
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>

bool Application::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	int windowWidth = 2048;
	int windowHeight = 1300;

	window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return false;
	}

	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) {glViewport(0, 0, width, height); });
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 440");
	ImGui::StyleColorsDark();

	//scene = std::make_shared<Scene>("scene");
	//scene = std::make_shared<SceneLight>("sceneLight");
	//scene = std::make_shared<SceneTexture>("sceneTexture");
	scene = std::make_shared<SceneFBO>("sceneFBO");
	scene->init(windowWidth, windowHeight);

	auto keyboardCallback = [](GLFWwindow* window, int key, int scancode, int action, int mode) {

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
			return;
		}

		auto self = static_cast<Application*>(glfwGetWindowUserPointer(window));
		if (self->scene)
			self->scene->keyboardCallback(key, scancode, action, mode);
	};

	auto mouseCallback = [](GLFWwindow* window, double xpos, double ypos) {
		auto self = static_cast<Application*>(glfwGetWindowUserPointer(window));
		if (self->scene)
			self->scene->mouseCallback(xpos, ypos);
	};

	glfwSetWindowUserPointer(window, static_cast<void*>(this));
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetCursorPosCallback(window, mouseCallback);

	return true;
}

void Application::run()
{
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrameTime = glfwGetTime();
	//auto lastFrameTimeChrono = std::chrono::system_clock::now();
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		//auto currentFrameTimeChrono = std::chrono::system_clock::now();
		//auto deltatimeChrono = currentFrameTimeChrono - lastFrameTimeChrono;
		//std::chrono::duration<float> difference = currentFrameTimeChrono - lastFrameTimeChrono;
		//float deltaTimeCrono = difference.count();
		//lastFrameTimeChrono = currentFrameTimeChrono;

		scene->update(deltaTime);
		//scene->update(deltatimeChrono.count());
		scene->draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Application::stop()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}
