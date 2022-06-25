#pragma once

#include <memory>
//#include <GLFW/glfw3.h>
//#include "gl_core_4_4.h"
//#include <string_view>

struct GLFWwindow;
class Scene;

class Application
{
public:
	bool init();
	void run();
	void stop();
private:
	GLFWwindow* window;	
	std::shared_ptr<Scene> scene;
};

