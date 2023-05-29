#include "SceneLight.h"
#include "ResourceManager/ResourceManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "Dependencies/imgui_impl_glfw.h"
#include "Dependencies/imgui_impl_opengl3.h"
#include "Object/torus.h"
#include "Object/teapot.h"
#include <sstream>
#include <iostream>


SceneLight::SceneLight(std::string_view tagScene)
	:Node(tagScene)
{}

void SceneLight::init(int aScreenWidth, int aScreenHeight)
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

	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

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
	ResourceManager::loadShader("../src/shader/discard_sprite.vs", "../src/shader/discard_sprite.fs", "discard_sprite");
	ResourceManager::loadShader("../src/shader/flat.vert", "../src/shader/flat.frag", "flat");
	ResourceManager::loadShader("../src/shader/twoside.vert", "../src/shader/twoside.frag", "twoside");
	ResourceManager::loadShader("../src/shader/chapter3/multilight.vert", "../src/shader/chapter3/multilight.frag", "multilight");
	ResourceManager::loadShader("../src/shader/chapter3/directional.vs", "../src/shader/chapter3/directional.fs", "directional");
	ResourceManager::loadShader("../src/shader/chapter3/perfrag.vs", "../src/shader/chapter3/perfrag.fs", "perfrag");
	ResourceManager::loadShader("../src/shader/chapter3/spot.vs", "../src/shader/chapter3/spot.fs", "spot");
	ResourceManager::loadShader("../src/shader/chapter3/toon.vs", "../src/shader/chapter3/toon.fs", "toon");
	ResourceManager::loadShader("../src/shader/chapter3/fog.vs", "../src/shader/chapter3/fog.fs", "fog");
	
	ResourceManager::loadTexture("../resources/textures/wall.jpg", GL_TRUE, "wall");


	//texture = ResourceManager::getTexture("wall");

	projection = glm::perspective(glm::radians(45.0f), screenWidth / static_cast<float>(screenHeight), 0.1f, 100.0f);

	lampModel = glm::mat4(1.0f);
	lampModel = glm::translate(lampModel, glm::vec3(1.2f, 1.0f, 2.0f));
	lampModel = glm::scale(lampModel, glm::vec3(0.2f));
	ResourceManager::getShader("lamp").setMatrix4("model", lampModel, true);

	pTorus = std::make_shared<Torus>(0.7f, 0.3f, 50, 50);
	pTeapot = std::make_shared<Teapot>(13, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)));
	pMesh = ObjMesh::load("../media/pig_triangulated.obj", true);

	std::vector<std::string> shaderName{ "flat" , "twoside" };
	for (auto name : shaderName) {
		ResourceManager::getShader(name).use();
		ResourceManager::getShader(name).setVector3f("Material.Kd", 0.9f, 0.5f, 0.3f);
		ResourceManager::getShader(name).setVector3f("Light.Ld", 1.0f, 1.0f, 1.0f);
		ResourceManager::getShader(name).setVector3f("Material.Ka", 0.9f, 0.5f, 0.3f);
		ResourceManager::getShader(name).setVector3f("Light.La", 0.4f, 0.4f, 0.4f);
		ResourceManager::getShader(name).setVector3f("Material.Ks", 0.8f, 0.8f, 0.8f);
		ResourceManager::getShader(name).setVector3f("Light.Ls", 1.0f, 1.0f, 1.0f);
		ResourceManager::getShader(name).setFloat("Material.Shininess", 100.0f);
	}

	ResourceManager::getShader("multilight").use();
	
	float x, z;
	for (int i = 0; i < 5; i++) {
		x = 2.0f * cosf((glm::two_pi<float>() / 5) * i);
		z = 2.0f * sinf((glm::two_pi<float>() / 5) * i);
		lightPosVector.emplace_back(x, z);
	}

	ResourceManager::getShader("multilight").setVector3f("lights[0].Intensity", glm::vec3(0.0f, 0.8f, 0.8f));
	ResourceManager::getShader("multilight").setVector3f("lights[1].Intensity", glm::vec3(0.0f, 0.0f, 0.8f));
	ResourceManager::getShader("multilight").setVector3f("lights[2].Intensity", glm::vec3(0.8f, 0.0f, 0.0f));
	ResourceManager::getShader("multilight").setVector3f("lights[3].Intensity", glm::vec3(0.0f, 0.8f, 0.0f));
	ResourceManager::getShader("multilight").setVector3f("lights[4].Intensity", glm::vec3(0.8f, 0.8f, 0.8f));

	ResourceManager::getShader("multilight").setVector3f("Kd", 0.4f, 0.4f, 0.4f);
	ResourceManager::getShader("multilight").setVector3f("Ks", 0.9f, 0.9f, 0.9f);
	ResourceManager::getShader("multilight").setVector3f("Ka", 0.1f, 0.1f, 0.1f);
	ResourceManager::getShader("multilight").setFloat("Shininess", 180.0f);

	ResourceManager::getShader("directional").use();
	ResourceManager::getShader("directional").setVector3f("LightIntensity", glm::vec3(0.8f, 0.8f, 0.8f));
	ResourceManager::getShader("directional").setVector3f("Kd", 0.8f, 0.8f, 0.8f);
	ResourceManager::getShader("directional").setVector3f("Ks", 0.9f, 0.9f, 0.9f);
	ResourceManager::getShader("directional").setVector3f("Ka", 0.1f, 0.1f, 0.1f);
	ResourceManager::getShader("directional").setFloat("Shininess", 180.0f);

	ResourceManager::getShader("perfrag").use();
	ResourceManager::getShader("perfrag").setVector3f("LightIntensity", glm::vec3(0.9f, 0.9f, 0.9f));
	ResourceManager::getShader("perfrag").setVector3f("Kd", 0.9f, 0.5f, 0.3f);
	ResourceManager::getShader("perfrag").setVector3f("Ks", 0.95f, 0.95f, 0.95f);
	ResourceManager::getShader("perfrag").setVector3f("Ka", 0.1f, 0.1f, 0.1f);
	ResourceManager::getShader("perfrag").setFloat("Shininess", 100.0f);

	ResourceManager::getShader("spot").use();
	ResourceManager::getShader("spot").setVector3f("Spot.intensity", glm::vec3(0.9f, 0.9f, 0.9f));
	ResourceManager::getShader("spot").setFloat("Spot.exponent", 30.0f);
	ResourceManager::getShader("spot").setFloat("Spot.cutoff", 15.0f);

	ResourceManager::getShader("toon").use();
	ResourceManager::getShader("toon").setVector3f("Light.intensity", glm::vec3(0.9f, 0.9f, 0.9f));

	ResourceManager::getShader("fog").use();
	ResourceManager::getShader("fog").setVector3f("Light.intensity", glm::vec3(0.9f, 0.9f, 0.9f));
	ResourceManager::getShader("fog").setFloat("Fog.maxDist", 30.0f);
	ResourceManager::getShader("fog").setFloat("Fog.minDist", 1.0f);
	ResourceManager::getShader("fog").setVector3f("Fog.color", glm::vec3(0.5f, 0.5f, 0.5f));

	ResourceManager::getShader("fog").setVector3f("Kd", 0.9f, 0.5f, 0.3f);
	ResourceManager::getShader("fog").setVector3f("Ka", 0.9f * 0.3f, 0.5f * 0.3f, 0.3f * 0.3f);
	ResourceManager::getShader("fog").setVector3f("Ks", 0.0f, 0.0f, 0.0f);
	ResourceManager::getShader("fog").setFloat("Shininess", 180.0f);

}

void SceneLight::keyboardCallback(int key, int scancode, int action, int mode)
{
	camera.processKeyboard(key, action);
}

void SceneLight::mouseCallback(double xpos, double ypos)
{
	camera.processMouseMovement(xpos, ypos);
}

void SceneLight::update(GLfloat dt)
{
	camera.updateCameraVectors(dt);

	ResourceManager::getShader("lamp").setMatrix4("view", camera.getViewMatrix(), true);
	ResourceManager::getShader("lamp").setMatrix4("projection", projection, true);

	angle += 0.25f * dt;
	if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
}

void SceneLight::imGuiNewFrame()
{
	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void SceneLight::inGuiRenderGUI()
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

void SceneLight::draw()
{
	imGuiNewFrame();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
		glm::mat4 modelLight(glm::mat4(1.0f));
		modelLight = glm::translate(modelLight, glm::vec3(1.2f, 1.0f, 2.0f));

		ResourceManager::getShader("twoside").use();
		ResourceManager::getShader("twoside").setVector4f("Light.Position", camera.getViewMatrix() * modelLight * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		glm::mat4 model;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(2.5f, 1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));

		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("twoside").setMatrix4("ModelViewMatrix", mv);
		ResourceManager::getShader("twoside").setMatrix3("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("twoside").setMatrix4("MVP", projection * mv);

		if (pTeapot)
			pTeapot->render();
	}

	{
		ResourceManager::getShader("multilight").use();

		lightPosVector.clear();
		float x, z;
		for (int i = 0; i < 5; i++) {
			x = 2.0f * cosf((glm::two_pi<float>() / 5 + angle) * i);
			z = 2.0f * sinf((glm::two_pi<float>() / 5 + angle) * i);
			lightPosVector.emplace_back(x, z);
		}

		for (int i = 0; i < 5; i++) {
			std::stringstream name;
			name << "lights[" << i << "].Position";
			float x = lightPosVector[i].x;
			float z = lightPosVector[i].z;
			ResourceManager::getShader("multilight").setVector4f(name.str().c_str(), camera.getViewMatrix() * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));
		}

		glm::mat4 model;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("multilight").setMatrix4("ModelViewMatrix", mv);
		ResourceManager::getShader("multilight").setMatrix3("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("multilight").setMatrix4("MVP", projection * mv);

		//if (pTeapot)
		//	pTeapot->render();

		if (pMesh)
			pMesh->render();

		ResourceManager::getShader("lamp").use();
		for (auto&& item : lightPosVector) {
			lampModel = glm::mat4(1.0f);
			lampModel = glm::translate(lampModel, glm::vec3(item.x, 1.0f, item.z));
			lampModel = glm::scale(lampModel, glm::vec3(0.1f));
			ResourceManager::getShader("lamp").setMatrix4("model", lampModel, true);

			glBindVertexArray(lampVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}

	{
		ResourceManager::getShader("directional").use();
		ResourceManager::getShader("directional").setVector4f("LightPosition", camera.getViewMatrix() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));

		glm::mat4 model;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.5f));

		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("directional").setMatrix4("ModelViewMatrix", mv);
		ResourceManager::getShader("directional").setMatrix3("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("directional").setMatrix4("MVP", projection * mv);

		if (pTorus)
			pTorus->render();
	}

	{
		glm::mat4 modelLight(glm::mat4(1.0f));
		modelLight = glm::translate(modelLight, glm::vec3(1.2f, 1.0f, 2.0f));

		ResourceManager::getShader("perfrag").use();
		ResourceManager::getShader("perfrag").setVector4f("LightPosition", camera.getViewMatrix() * modelLight * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		//ResourceManager::getShader("perfrag").use();
		//ResourceManager::getShader("perfrag").setVector4f("LightPosition", camera.getViewMatrix() * glm::vec4(1.2f, 1.0f, 2.0f, 0.0f));

		glm::mat4 model;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(5.5f, 1.0f, 0.0));
		//model = glm::scale(model, glm::vec3(0.5f));

		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("perfrag").setMatrix4("ModelViewMatrix", mv);
		ResourceManager::getShader("perfrag").setMatrix3("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("perfrag").setMatrix4("MVP", projection * mv);

		if (pTorus)
			pTorus->render();
	}

	{
		
		glm::vec4 lightPos = glm::vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
		ResourceManager::getShader("spot").use();
		ResourceManager::getShader("spot").setVector4f("Spot.position", camera.getViewMatrix()* lightPos);
		glm::mat3 normalMatrix = glm::mat3(	glm::vec3(camera.getViewMatrix()[0]), 
											glm::vec3(camera.getViewMatrix()[1]), 
											glm::vec3(camera.getViewMatrix()[2]));
		ResourceManager::getShader("spot").setVector3f("Spot.direction", normalMatrix * glm::vec3(-lightPos));

		ResourceManager::getShader("spot").setVector3f("Kd", 0.9f, 0.5f, 0.3f);
		ResourceManager::getShader("spot").setVector3f("Ks", 0.95f, 0.95f, 0.95f);
		ResourceManager::getShader("spot").setVector3f("Ka", 0.9f * 0.3f, 0.5f * 0.3f, 0.3f * 0.3f);
		ResourceManager::getShader("spot").setFloat("Shininess", 100.0f);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -2.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("spot").setMatrix4("ModelViewMatrix", mv);
		ResourceManager::getShader("spot").setMatrix3("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("spot").setMatrix4("MVP", projection* mv);

		if (pTeapot)
			pTeapot->render();

		ResourceManager::getShader("spot").setVector3f("Kd", 0.9f, 0.5f, 0.3f);
		ResourceManager::getShader("spot").setVector3f("Ks", 0.95f, 0.95f, 0.95f);
		ResourceManager::getShader("spot").setVector3f("Ka", 0.9f * 0.3f, 0.5f * 0.3f, 0.3f * 0.3f);
		ResourceManager::getShader("spot").setFloat("Shininess", 100.0f);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, 0.75f, 3.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("spot").setMatrix4("ModelViewMatrix", mv);
		ResourceManager::getShader("spot").setMatrix3("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("spot").setMatrix4("MVP", projection* mv);

		if (pTorus)
			pTorus->render();


		ResourceManager::getShader("lamp").use();
		lampModel = glm::mat4(1.0f);
		lampModel = glm::translate(lampModel, glm::vec3(lightPos.x, lightPos.y, lightPos.z));
		lampModel = glm::scale(lampModel, glm::vec3(0.1f));
		ResourceManager::getShader("lamp").setMatrix4("model", lampModel, true);

		glBindVertexArray(lampVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	{

		glm::vec4 lightPos = glm::vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
		ResourceManager::getShader("toon").use();
		ResourceManager::getShader("toon").setVector4f("Light.position", camera.getViewMatrix() * lightPos);
		glm::mat3 normalMatrix = glm::mat3(glm::vec3(camera.getViewMatrix()[0]),
			glm::vec3(camera.getViewMatrix()[1]),
			glm::vec3(camera.getViewMatrix()[2]));
		ResourceManager::getShader("toon").setVector3f("Spot.direction", normalMatrix * glm::vec3(-lightPos));

		ResourceManager::getShader("toon").setVector3f("Kd", 0.9f, 0.5f, 0.3f);
		ResourceManager::getShader("toon").setVector3f("Ka", 0.9f * 0.3f, 0.5f * 0.3f, 0.3f * 0.3f);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, -2.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("toon").setMatrix4("ModelViewMatrix", mv);
		ResourceManager::getShader("toon").setMatrix3("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("toon").setMatrix4("MVP", projection * mv);

		if (pTeapot)
			pTeapot->render();

		ResourceManager::getShader("toon").setVector3f("Kd", 0.9f, 0.5f, 0.3f);
		ResourceManager::getShader("toon").setVector3f("Ka", 0.9f * 0.3f, 0.5f * 0.3f, 0.3f * 0.3f);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 0.75f, 3.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("toon").setMatrix4("ModelViewMatrix", mv);
		ResourceManager::getShader("toon").setMatrix3("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("toon").setMatrix4("MVP", projection * mv);

		if (pTorus)
			pTorus->render();

		//ResourceManager::getShader("lamp").use();
		//lampModel = glm::mat4(1.0f);
		//lampModel = glm::translate(lampModel, glm::vec3(lightPos.x, lightPos.y, lightPos.z));
		//lampModel = glm::scale(lampModel, glm::vec3(0.1f));
		//ResourceManager::getShader("lamp").setMatrix4("model", lampModel, true);

		//glBindVertexArray(lampVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);
	}

	{
		glm::vec4 lightPos = glm::vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
		ResourceManager::getShader("fog").use();
		ResourceManager::getShader("fog").setVector4f("Light.position", camera.getViewMatrix()* lightPos);

		float dist = 0.0f;
		for (int i = 0; i < 4; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(dist * 0.6f - 1.0f, -3.0f, -dist));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			
			glm::mat4 mv = camera.getViewMatrix() * model;
			ResourceManager::getShader("fog").setMatrix4("ModelViewMatrix", mv);
			ResourceManager::getShader("fog").setMatrix3("NormalMatrix",
				glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
			ResourceManager::getShader("fog").setMatrix4("MVP", projection* mv);

			if (pTeapot)
				pTeapot->render();

			dist += 7.0f;
		}
	}

	{
		ResourceManager::getShader("lamp").use();

		lampModel = glm::mat4(1.0f);
		lampModel = glm::translate(lampModel, glm::vec3(1.2f, 1.0f, 2.0f));
		lampModel = glm::scale(lampModel, glm::vec3(0.2f));
		ResourceManager::getShader("lamp").setMatrix4("model", lampModel, true);

		glBindVertexArray(lampVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	inGuiRenderGUI();
}