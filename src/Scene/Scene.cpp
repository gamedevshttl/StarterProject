#include "Scene.h"
#include "ResourceManager/ResourceManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "Dependencies/imgui_impl_glfw.h"
#include "Dependencies/imgui_impl_opengl3.h"


Scene::Scene(std::string_view tagScene)
	:Node(tagScene)
{}

void Scene::init()
{
	GLfloat positionData[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	GLfloat colorData[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};

	GLuint vboPosition;
	glGenBuffers(1, &vboPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positionData), positionData, GL_STATIC_DRAW);

	GLuint vboColor;
	glGenBuffers(1, &vboColor);
	glBindBuffer(GL_ARRAY_BUFFER, vboColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);

	//GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vboColor);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindVertexArray(0);

	ResourceManager::loadShader("../src/shader/sprite.vs", "../src/shader/sprite.fs", "sprite");

	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	//ResourceManager::getShader("sprite").use();
	//GLuint location = glGetUniformLocation(ResourceManager::getShader("sprite").getId(), "rotationMatrix");
	//glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(rotation));

	ResourceManager::getShader("sprite").setMatrix4("rotationMatrix", rotation, true);
}

void Scene::update(GLfloat dt)
{

}

void Scene::imGuiNewFrame()
{
	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Scene::inGuiRenderGUI()
{
	// render your GUI
	ImGui::Begin("Demo window");
	ImGui::Button("Hello!");
	ImGui::SliderFloat("rotation", &rotation, 0, 2 * 3.14);
	ImGui::End();

	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Scene::draw()
{
	imGuiNewFrame();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//shader.use();
	ResourceManager::getShader("sprite").use();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	inGuiRenderGUI();
}