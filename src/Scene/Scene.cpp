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

void Scene::init(int aScreenWidth, int aScreenHeight)
{
	screenWidth = aScreenWidth;
	screenHeight = aScreenHeight;
	camera.init(aScreenWidth / 2.f, aScreenHeight / 2.f);

	std::vector<GLfloat> positionData = ResourceManager::loadData("../resources/data/cube_position.json");
	std::vector<GLfloat> texCoords = ResourceManager::loadData("../resources/data/cube_texture.json");

	GLuint vboPosition;
	glGenBuffers(1, &vboPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
	glBufferData(GL_ARRAY_BUFFER, positionData.size() * sizeof(GLfloat), &positionData.front(), GL_STATIC_DRAW);

	GLuint vboTexture;
	glGenBuffers(1, &vboTexture);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexture);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords), &texCoords.front(), GL_STATIC_DRAW);

	//GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//glBindVertexArray(0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexture);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);


	glGenVertexArrays(1, &lampVAO);
	glBindVertexArray(lampVAO);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);


	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);

	ResourceManager::loadShader("../src/shader/sprite.vs", "../src/shader/sprite.fs", "sprite");
	ResourceManager::loadShader("../src/shader/lamp.vs", "../src/shader/lamp.fs", "lamp");
	ResourceManager::loadTexture("../resources/textures/wall.jpg", GL_TRUE, "wall");

	ResourceManager::getShader("sprite").setMatrix4("rotationMatrix", rotationMatrix, true);
	ResourceManager::getShader("sprite").setVector3f("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
	ResourceManager::getShader("sprite").setVector3f("lightColor", 1.0f, 1.0f, 1.0f);

	texture = ResourceManager::getTexture("wall");

	model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	projection = glm::perspective(glm::radians(45.0f), screenWidth / static_cast<float>(screenHeight), 0.1f, 100.0f);

	lampModel = glm::mat4(1.0f);
	lampModel = glm::translate(lampModel, glm::vec3(1.2f, 1.0f, 2.0f));
	lampModel = glm::scale(lampModel, glm::vec3(0.2f));
	ResourceManager::getShader("lamp").setMatrix4("model", lampModel, true);

	GLint numUniforms = 0;
	glGetProgramInterfaceiv(ResourceManager::getShader("sprite").getId(), GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
}

void Scene::keyboardCallback(int key, int scancode, int action, int mode)
{
	camera.processKeyboard(key, action);
}

void Scene::mouseCallback(double xpos, double ypos)
{
	camera.processMouseMovement(xpos, ypos);
}

void Scene::update(GLfloat dt)
{
	model = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));

	camera.updateCameraVectors(dt);

	ResourceManager::getShader("sprite").setMatrix4("model", model, true);
	ResourceManager::getShader("sprite").setMatrix4("view", camera.getViewMatrix(), true);
	ResourceManager::getShader("sprite").setMatrix4("projection", projection, true);
	ResourceManager::getShader("sprite").setVector3f("viewPos", camera.getPosition());

	ResourceManager::getShader("lamp").setMatrix4("view", camera.getViewMatrix(), true);
	ResourceManager::getShader("lamp").setMatrix4("projection", projection, true);
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
	ImGui::Begin("Setting");
	if (ImGui::Button("Reset"))
	{
		rotation = 0;
	}
	ImGui::SliderFloat("rotation", &rotation, 0, 360);
	ImGui::End();

	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Scene::draw()
{
	imGuiNewFrame();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ResourceManager::getShader("sprite").use();

	glActiveTexture(GL_TEXTURE0);
	texture.bind();
	glUniform1i(glGetUniformLocation(texture.id, "image"), 0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);


	ResourceManager::getShader("lamp").use();
	glBindVertexArray(lampVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	inGuiRenderGUI();
}