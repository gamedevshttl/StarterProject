#include "SceneFBO.h"
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


SceneFBO::SceneFBO(std::string_view tagScene)
	:SceneBase(tagScene)
{}

void SceneFBO::init(int aScreenWidth, int aScreenHeight)
{
	screenWidth = aScreenWidth;
	screenHeight = aScreenHeight;
	camera.init(aScreenWidth / 2.f, aScreenHeight / 2.f);

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	ResourceManager::loadShader("../src/shader/chapter5/edge.vs", "../src/shader/chapter5/edge.fs", "edge");

	projection = glm::perspective(glm::radians(45.0f), screenWidth / static_cast<float>(screenHeight), 0.1f, 100.0f);
	projection = glm::mat4(1.0f);


	pTorus = std::make_shared<Torus>(0.7f, 0.3f, 50, 50);
	pTeapot = std::make_shared<Teapot>(13, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)));
	pMesh = ObjMesh::load("../media/pig_triangulated.obj", true);
	pCube = std::make_shared<Cube>();
	pOgre = ObjMesh::load("../media/bs_ears.obj", false, true);
	pSkyBox = std::make_shared<SkyBox>();


	// Generate and bind the framebuffer
	glGenFramebuffers(1, &fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

	// Create the texture object
	glGenTextures(1, &renderTex);
	glBindTexture(GL_TEXTURE_2D, renderTex);
#ifdef __APPLE__
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
#else
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, screenWidth, screenHeight);
#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	// Bind the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

	// Create the depth buffer
	GLuint depthBuf;
	glGenRenderbuffers(1, &depthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);

	// Bind the depth buffer to the FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, depthBuf);

	// Set the targets for the fragment output variables
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Array for full-screen quad
	GLfloat verts[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
	};
	GLfloat tc[] = {
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	unsigned int handle[2];
	glGenBuffers(2, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), tc, GL_STATIC_DRAW);

	glGenVertexArrays(1, &fsQuad);
	glBindVertexArray(fsQuad);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	ResourceManager::getShader("edge").use();
	// Set up the subroutine indexes
	GLuint programHandle = ResourceManager::getShader("edge").getId();
	pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "pass1");
	pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "pass2");

	ResourceManager::getShader("edge").setUniform("EdgeThreshold", 0.05f);
	ResourceManager::getShader("edge").setUniform("Light.Intensity", glm::vec3(1.0f, 1.0f, 1.0f));
}

void SceneFBO::keyboardCallback(int key, int scancode, int action, int mode)
{
	camera.processKeyboard(key, action);
}

void SceneFBO::mouseCallback(double xpos, double ypos)
{
	camera.processMouseMovement(xpos, ypos);
}

void SceneFBO::update(GLfloat dt)
{
	camera.updateCameraVectors(dt);

	angle += 0.25f * dt;
	if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
}

void SceneFBO::imGuiNewFrame()
{
	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void SceneFBO::inGuiRenderGUI()
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


void SceneFBO::draw()
{
	imGuiNewFrame();

	ResourceManager::getShader("edge").use();
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
		glEnable(GL_DEPTH_TEST);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);

		//view = glm::lookAt(vec3(7.0f * cos(angle), 4.0f, 7.0f * sin(angle)), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projection = glm::perspective(glm::radians(45.0f), screenWidth / static_cast<float>(screenHeight), 0.1f, 100.0f);

		ResourceManager::getShader("edge").setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		ResourceManager::getShader("edge").setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
		ResourceManager::getShader("edge").setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
		ResourceManager::getShader("edge").setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
		ResourceManager::getShader("edge").setUniform("Material.Shininess", 100.0f);

		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		//setMatrices();
		glm::mat4 mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("edge").setUniform("ModelViewMatrix", mv);
		ResourceManager::getShader("edge").setUniform("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("edge").setUniform("MVP", projection * mv);

		if (pTeapot)
			pTeapot->render();

		ResourceManager::getShader("edge").setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		ResourceManager::getShader("edge").setUniform("Material.Kd", 0.9f, 0.5f, 0.2f);
		ResourceManager::getShader("edge").setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
		ResourceManager::getShader("edge").setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
		ResourceManager::getShader("edge").setUniform("Material.Shininess", 100.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, 1.0f, 3.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		//setMatrices();
		mv = camera.getViewMatrix() * model;
		ResourceManager::getShader("edge").setUniform("ModelViewMatrix", mv);
		ResourceManager::getShader("edge").setUniform("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("edge").setUniform("MVP", projection * mv);

		if (pTorus)
			pTorus->render();
	}
	glFlush();
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTex);

		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);

		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);
		model = glm::mat4(1.0f);
		view = glm::mat4(1.0f);
		projection = glm::mat4(1.0f);
		//setMatrices();
		glm::mat4 mv = view * model;
		ResourceManager::getShader("edge").setUniform("ModelViewMatrix", mv);
		ResourceManager::getShader("edge").setUniform("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		ResourceManager::getShader("edge").setUniform("MVP", projection * mv);

		// Render the full-screen quad
		glBindVertexArray(fsQuad);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	inGuiRenderGUI();
}