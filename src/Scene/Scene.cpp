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

	GLfloat texCoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.5f, 1.0f
	};


	GLuint vboPosition;
	glGenBuffers(1, &vboPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positionData), positionData, GL_STATIC_DRAW);

	GLuint vboColor;
	glGenBuffers(1, &vboColor);
	glBindBuffer(GL_ARRAY_BUFFER, vboColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);

	GLuint vboTexture;
	glGenBuffers(1, &vboTexture);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

	//GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vboColor);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexture);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);

	ResourceManager::loadShader("../src/shader/sprite.vs", "../src/shader/sprite.fs", "sprite");
	ResourceManager::loadTexture("../resources/textures/wall.jpg", GL_TRUE, "wall");

	ResourceManager::getShader("sprite").setMatrix4("rotationMatrix", rotationMatrix, true);
	texture = ResourceManager::getTexture("wall");

	model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	projection = glm::perspective(glm::radians(45.0f), screenWidth / static_cast<float>(screenHeight), 0.1f, 100.0f);
}

void Scene::update(GLfloat dt)
{
	//rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	//ResourceManager::getShader("sprite").setMatrix4("rotationMatrix", rotationMatrix, true);

	model = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));

	ResourceManager::getShader("sprite").setMatrix4("model", model, true);
	ResourceManager::getShader("sprite").setMatrix4("view", view, true);
	ResourceManager::getShader("sprite").setMatrix4("projection", projection, true);

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
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	inGuiRenderGUI();
}