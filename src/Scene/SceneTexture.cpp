#include "SceneTexture.h"
#include "ResourceManager/ResourceManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "Dependencies/imgui_impl_glfw.h"
#include "Dependencies/imgui_impl_opengl3.h"
#include "Object/torus.h"
#include "Object/teapot.h"
#include "Object/cube.h"
#include "Object/skybox.h"
#include <sstream>
#include <iostream>


SceneTexture::SceneTexture(std::string_view tagScene)
	:SceneBase(tagScene)
{}

void SceneTexture::init(int aScreenWidth, int aScreenHeight)
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

	ResourceManager::loadShader("../src/shader/lamp.vs", "../src/shader/lamp.fs", "lamp");
	
	ResourceManager::loadShader("../src/shader/chapter4/texture.vs", "../src/shader/chapter4/texture.fs", "texture");
	ResourceManager::loadShader("../src/shader/chapter4/multitex.vs", "../src/shader/chapter4/multitex.fs", "multitex");
	ResourceManager::loadShader("../src/shader/chapter4/alphatest.vs", "../src/shader/chapter4/alphatest.fs", "alphatest");
	ResourceManager::loadShader("../src/shader/chapter4/normalmap.vs", "../src/shader/chapter4/normalmap.fs", "normalmap");
	ResourceManager::loadShader("../src/shader/chapter4/cubemap_refract.vs", "../src/shader/chapter4/cubemap_refract.fs", "cubemap_refract");
	ResourceManager::loadShader("../src/shader/chapter4/cubemap_reflect.vs", "../src/shader/chapter4/cubemap_reflect.fs", "cubemap_reflect");
	ResourceManager::loadShader("../src/shader/chapter4/projtex.vs", "../src/shader/chapter4/projtex.fs", "projtex");
	ResourceManager::loadShader("../src/shader/chapter4/rendertotex.vs", "../src/shader/chapter4/rendertotex.fs", "rendertotex");

	ResourceManager::loadTexture("../resources/textures/wall.jpg", GL_TRUE, "wall");
	ResourceManager::loadTexture("../media/texture/brick1.jpg", GL_TRUE, "brick");
	texture = ResourceManager::getTexture("brick");
	ResourceManager::loadTexture("../media/texture/moss.png", GL_TRUE, "moss");

	// Load diffuse texture
	ResourceManager::loadTexture("../media/texture/ogre_diffuse.png", GL_TRUE, "ogre_diffuse");
	// Load normal map
	ResourceManager::loadTexture("../media/texture/ogre_normalmap.png", GL_TRUE, "ogre_normalmap");

	ResourceManager::loadCubeMap("../media/texture/cubemap_night/night", "night_sky_box");

	ResourceManager::loadTexture("../media/texture/flower.png", GL_TRUE, "flower");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	projection = glm::perspective(glm::radians(45.0f), screenWidth / static_cast<float>(screenHeight), 0.1f, 100.0f);

	lampModel = glm::mat4(1.0f);
	lampModel = glm::translate(lampModel, glm::vec3(1.2f, 1.0f, 2.0f));
	lampModel = glm::scale(lampModel, glm::vec3(0.2f));
	ResourceManager::getShader("lamp").setMatrix4("model", lampModel, true);

	pTorus = std::make_shared<Torus>(0.7f, 0.3f, 50, 50);
	pTeapot = std::make_shared<Teapot>(13, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)));
	pMesh = ObjMesh::load("../media/pig_triangulated.obj", true);
	pCube = std::make_shared<Cube>();
	pOgre = ObjMesh::load("../media/bs_ears.obj", false, true);
	pSkyBox = std::make_shared<SkyBox>();

	ResourceManager::getShader("texture").use();
	ResourceManager::getShader("texture").setUniform("Light.Intensity", glm::vec3(1.0f, 1.0f, 1.0f));

	ResourceManager::getShader("multitex").use();
	ResourceManager::getShader("multitex").setUniform("Light.Intensity", glm::vec3(1.0f, 1.0f, 1.0f));

	ResourceManager::getShader("alphatest").use();
	ResourceManager::getShader("alphatest").setUniform("Light.Intensity", glm::vec3(1.0f, 1.0f, 1.0f));

	ResourceManager::getShader("normalmap").use();
	ResourceManager::getShader("normalmap").setUniform("Light.Intensity", glm::vec3(0.9f, 0.9f, 0.9f));

	ResourceManager::getShader("projtex").use();
	ResourceManager::getShader("projtex").setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	ResourceManager::getShader("projtex").setUniform("Light.Intensity", glm::vec3(1.0f, 1.0f, 1.0f));

	glm::vec3 projPos = glm::vec3(2.0f, 5.0f, 5.0f);
	glm::vec3 projAt = glm::vec3(-2.0f, -4.0f, 0.0f);
	glm::vec3 projUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 projView = glm::lookAt(projPos, projAt, projUp);
	glm::mat4 projProj = glm::perspective(glm::radians(30.0f), 1.0f, 0.2f, 1000.0f);
	glm::mat4 projScaleTrans = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	ResourceManager::getShader("projtex").setUniform("ProjectorMatrix", projScaleTrans * projProj * projView);

	ResourceManager::getShader("rendertotex").use();
	ResourceManager::getShader("rendertotex").setUniform("Light.Intensity", glm::vec3(1.0f, 1.0f, 1.0f));

	glGenFramebuffers(1, &fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

	glGenTextures(1, &renderTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

	// Create the depth buffer
	GLuint depthBuf;
	glGenRenderbuffers(1, &depthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);

	// Bind the depth buffer to the FBO
	glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, depthBuf);

	// Set the targets for the fragment output variables
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "framebuffer is complete" << std::endl;
	else
		std::cout << "framebuffer error " << result << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// One pixel white texture
	GLubyte whiteTex[] = { 255, 255, 255, 255 };
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &whiteTexHandle);
	glBindTexture(GL_TEXTURE_2D, whiteTexHandle);
#ifdef __APPLE__
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
#else
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1, 1);
#endif
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteTex);
}

void SceneTexture::keyboardCallback(int key, int scancode, int action, int mode)
{
	camera.processKeyboard(key, action);
}

void SceneTexture::mouseCallback(double xpos, double ypos)
{
	camera.processMouseMovement(xpos, ypos);
}

void SceneTexture::update(GLfloat dt)
{
	camera.updateCameraVectors(dt);

	ResourceManager::getShader("lamp").setMatrix4("view", camera.getViewMatrix(), true);
	ResourceManager::getShader("lamp").setMatrix4("projection", projection, true);

	angle += 0.25f * dt;
	if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
}

void SceneTexture::imGuiNewFrame()
{
	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void SceneTexture::inGuiRenderGUI()
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

void SceneTexture::draw()
{
	imGuiNewFrame();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{	
		glm::vec4 lightPos = glm::vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);

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
		ResourceManager::getShader("texture").use();
		
		glm::vec4 lightPos = glm::vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
		ResourceManager::getShader("texture").setVector4f("Light.Position", camera.getViewMatrix() * lightPos);

		ResourceManager::getShader("texture").setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
		ResourceManager::getShader("texture").setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
		ResourceManager::getShader("texture").setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
		ResourceManager::getShader("texture").setUniform("Material.Shininess", 100.0f);

		glActiveTexture(GL_TEXTURE0);
		texture.bind();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("texture").setMatrix4("ModelViewMatrix", mv);
		ResourceManager::getShader("texture").setMatrix3("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("texture").setMatrix4("MVP", projection* mv);

		if (pCube)
			pCube->render();
	}

	{
		ResourceManager::getShader("multitex").use();

		glm::vec4 lightPos = glm::vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
		ResourceManager::getShader("multitex").setVector4f("Light.Position", camera.getViewMatrix() * lightPos);

		ResourceManager::getShader("multitex").setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
		ResourceManager::getShader("multitex").setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
		ResourceManager::getShader("multitex").setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
		ResourceManager::getShader("multitex").setUniform("Material.Shininess", 100.0f);

		glActiveTexture(GL_TEXTURE0);
		auto brickTexture = ResourceManager::getTexture("brick");
		brickTexture.bind();

		glActiveTexture(GL_TEXTURE1);
		auto mossTexture = ResourceManager::getTexture("moss");
		mossTexture.bind();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("multitex").setMatrix4("ModelViewMatrix", mv);
		ResourceManager::getShader("multitex").setMatrix3("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("multitex").setMatrix4("MVP", projection * mv);

		if (pCube)
			pCube->render();
	}

	{
		ResourceManager::getShader("alphatest").use();

		glm::vec4 lightPos = glm::vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
		ResourceManager::getShader("alphatest").setVector4f("Light.Position", camera.getViewMatrix() * lightPos);

		ResourceManager::getShader("alphatest").setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
		ResourceManager::getShader("alphatest").setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
		ResourceManager::getShader("alphatest").setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
		ResourceManager::getShader("alphatest").setUniform("Material.Shininess", 100.0f);

		glActiveTexture(GL_TEXTURE0);
		auto brickTexture = ResourceManager::getTexture("brick");
		brickTexture.bind();

		glActiveTexture(GL_TEXTURE1);
		auto mossTexture = ResourceManager::getTexture("moss");
		mossTexture.bind();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("alphatest").setMatrix4("ModelViewMatrix", mv);
		ResourceManager::getShader("alphatest").setMatrix3("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("alphatest").setMatrix4("MVP", projection * mv);

		if (pCube)
			pCube->render();
	}

	{
		ResourceManager::getShader("normalmap").use();

		glm::vec4 lightPos = glm::vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
		ResourceManager::getShader("normalmap").setVector4f("Light.Position", camera.getViewMatrix() * lightPos);

		ResourceManager::getShader("normalmap").setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
		ResourceManager::getShader("normalmap").setUniform("Material.Ks", 0.2f, 0.2f, 0.2f);
		ResourceManager::getShader("normalmap").setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
		ResourceManager::getShader("normalmap").setUniform("Material.Shininess", 1.0f);
		
		glActiveTexture(GL_TEXTURE0);
		ResourceManager::getTexture("ogre_diffuse").bind();

		glActiveTexture(GL_TEXTURE1);
		ResourceManager::getTexture("ogre_normalmap").bind();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(8.0f, 0.0f, 0.0f));
		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("normalmap").setMatrix4("ModelViewMatrix", mv);
		ResourceManager::getShader("normalmap").setMatrix3("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("normalmap").setMatrix4("MVP", projection * mv);

		if (pOgre)
			pOgre->render();
	}

	{
		ResourceManager::getShader("cubemap_refract").use();
		
		ResourceManager::getShader("cubemap_refract").setUniform("WorldCameraPosition", camera.getViewMatrix());

		ResourceManager::getShader("cubemap_refract").setUniform("DrawSkyBox", true);
		model = glm::mat4(1.0f);
		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("cubemap_refract").setUniform("ModelMatrix", model);
		ResourceManager::getShader("cubemap_refract").setUniform("MVP", projection* mv);
		if(pSkyBox)
			pSkyBox->render();
		ResourceManager::getShader("cubemap_refract").setUniform("DrawSkyBox", false);

		ResourceManager::getShader("cubemap_refract").setUniform("Material.Eta", 0.94f);
		ResourceManager::getShader("cubemap_refract").setUniform("Material.ReflectionFactor", 0.1f);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.5f));
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("cubemap_refract").setUniform("ModelMatrix", model);
		ResourceManager::getShader("cubemap_refract").setUniform("MVP", projection* mv);

		if (pTeapot)
			pTeapot->render();
	}

	{
		ResourceManager::getShader("cubemap_reflect").use();
		ResourceManager::getShader("cubemap_reflect").setUniform("WorldCameraPosition", camera.getViewMatrix());

		ResourceManager::getShader("cubemap_reflect").setUniform("DrawSkyBox", true);
		model = glm::mat4(1.0f);
		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("cubemap_reflect").setUniform("ModelMatrix", model);
		ResourceManager::getShader("cubemap_reflect").setUniform("MVP", projection * mv);
		if (pSkyBox)
			pSkyBox->render();
		ResourceManager::getShader("cubemap_reflect").setUniform("DrawSkyBox", false);

		ResourceManager::getShader("cubemap_reflect").setUniform("MaterialColor", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		ResourceManager::getShader("cubemap_reflect").setUniform("ReflectFactor", 0.85f);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.5f));
		model = glm::translate(model, glm::vec3(-10.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("cubemap_reflect").setUniform("ModelMatrix", model);
		ResourceManager::getShader("cubemap_reflect").setUniform("MVP", projection * mv);

		if (pTeapot)
			pTeapot->render();
	}

	{
		ResourceManager::getShader("projtex").use();

		glActiveTexture(GL_TEXTURE0);
		ResourceManager::getTexture("flower").bind();

		ResourceManager::getShader("projtex").setUniform("Material.Kd", 0.5f, 0.2f, 0.1f);
		ResourceManager::getShader("projtex").setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
		ResourceManager::getShader("projtex").setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
		ResourceManager::getShader("projtex").setUniform("Material.Shininess", 100.0f);

		glm::vec4 pos = glm::vec4(1.0f * cos(angle), 0.0f, 1.0f * sin(angle), 1.0f);

		model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(pos.x, -1.0f, pos.z));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 1.0f, 0.0f));

		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("projtex").setUniform("ModelMatrix", model);
		ResourceManager::getShader("projtex").setUniform("ModelViewMatrix", mv);
		ResourceManager::getShader("projtex").setUniform("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("projtex").setUniform("MVP", projection* mv);

		if (pTeapot)
			pTeapot->render();

		for (int i = 0; i < 5; ++i) {
			float x = 2.0f * cosf((glm::two_pi<float>() / 5) * i);
			float z = 2.0f * sinf((glm::two_pi<float>() / 5) * i);
			glm::vec3 posCube = glm::vec3(x, -3.0f, z);

			model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(-1.0f + i, -2.0f, 0.0f));
			model = glm::translate(model, posCube);
			glm::mat4 mv = camera.getViewMatrix() * model;
			ResourceManager::getShader("projtex").setUniform("ModelMatrix", model);
			ResourceManager::getShader("projtex").setMatrix4("ModelViewMatrix", mv);
			ResourceManager::getShader("projtex").setMatrix3("NormalMatrix",
				glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
			ResourceManager::getShader("projtex").setMatrix4("MVP", projection * mv);

			if (pCube)
				pCube->render();
		}
	}

	{
		ResourceManager::getShader("rendertotex").use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, whiteTexHandle);

		glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
		ResourceManager::getShader("rendertotex").setUniform("RenderTex", 1);
		glViewport(0, 0, 512, 512);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view = glm::lookAt(glm::vec3(0.0f, 0.0f, 7.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.3f, 100.0f);

		ResourceManager::getShader("rendertotex").setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		ResourceManager::getShader("rendertotex").setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
		ResourceManager::getShader("rendertotex").setUniform("Material.Ks", 0.95f, 0.95f, 0.5f);
		ResourceManager::getShader("rendertotex").setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
		ResourceManager::getShader("rendertotex").setUniform("Material.Shininess", 100.0f);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		glm::vec3 cameraPos = glm::vec3(2.0f * cos(angle), 1.5f, 2.0f * sin(angle));
		glm::mat4 texView = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 mv = texView * model;
		ResourceManager::getShader("rendertotex").setUniform("ModelViewMatrix", mv);
		ResourceManager::getShader("rendertotex").setUniform("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("rendertotex").setUniform("MVP", projection* mv);

		if (pTeapot)
			pTeapot->render();
		glFlush();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		ResourceManager::getShader("rendertotex").setUniform("RenderTex", 0);
		glViewport(0, 0, screenWidth, screenHeight);
		projection = glm::perspective(glm::radians(45.0f), screenWidth / static_cast<float>(screenHeight), 0.1f, 100.0f);

		ResourceManager::getShader("rendertotex").setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		ResourceManager::getShader("rendertotex").setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
		ResourceManager::getShader("rendertotex").setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
		ResourceManager::getShader("rendertotex").setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
		ResourceManager::getShader("rendertotex").setUniform("Material.Shininess", 1.0f);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 1.0f, -5.0f));
		mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("rendertotex").setUniform("ModelViewMatrix", mv);
		ResourceManager::getShader("rendertotex").setUniform("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("rendertotex").setUniform("MVP", projection* mv);

		if (pCube)
			pCube->render();
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