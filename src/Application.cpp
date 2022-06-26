#include "Application.h"
#include "Scene/Scene.h"

#include "imgui.h"
#include "Dependencies/imgui_impl_glfw.h"
#include "Dependencies/imgui_impl_opengl3.h"

#include "glCore/gl_core_4_4.h"
#include <GLFW/glfw3.h>

#include <chrono>


bool Application::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	int windowWidth = 1024;
	int windowHeight = 768;

	window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return false;
	}

	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) {glViewport(0, 0, width, height); });

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 440");
	ImGui::StyleColorsDark();

	scene = std::make_shared<Scene>("scene");
	scene->init();

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
