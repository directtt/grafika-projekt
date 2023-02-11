#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include "SOIL/SOIL.h"

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

int WIDTH = 500, HEIGHT = 500;

namespace models {
	Core::RenderContext bedContext;
	Core::RenderContext chairContext;
	Core::RenderContext deskContext;
	Core::RenderContext doorContext;
	Core::RenderContext drawerContext;
	Core::RenderContext marbleBustContext;
	Core::RenderContext materaceContext;
	Core::RenderContext pencilsContext;
	Core::RenderContext planeContext;
	Core::RenderContext roomContext;
	Core::RenderContext spaceshipContext;
	Core::RenderContext sphereContext;
	Core::RenderContext windowContext;
	Core::RenderContext testContext;
	Core::RenderContext treeContext;
	Core::RenderContext treeTrunkContext;
	Core::RenderContext starContext;
	Core::RenderContext table2Context;
	Core::RenderContext christmasPresent;
	Core::RenderContext christmasPresent2;
	Core::RenderContext christmasPresent3;
	Core::RenderContext christmasPresent4;
	Core::RenderContext monitor;
	Core::RenderContext keyboard;
	Core::RenderContext computerMouse;
	Core::RenderContext pillow;
	Core::RenderContext carpet;
}

namespace texture {
	GLuint cubemapTexture;
	GLuint earth;
	GLuint tableWood;
	GLuint sleigh;
	GLuint present;
	GLuint present2;
	GLuint present3;
	GLuint present4;
	GLuint monitor;
	GLuint keyboard;
	GLuint moon;
	GLuint pillow;
	GLuint carpet;
}

namespace normals {
	GLuint normal_present2;
	GLuint normal_present3;
}

struct Triplet
{
	float x, y, z;
};

GLuint depthMapFBO;
GLuint depthMap;

GLuint sleighDepthMap;
GLuint sleighDepthMapFBO;

GLuint program;
GLuint programSun;
GLuint programTest;
GLuint programTex;
GLuint programTexNormal;
GLuint programSkybox;
GLuint programDepth;

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext skyboxContext;

glm::vec3 sunPos = glm::vec3(-4.740971f, 2.149999f, 0.369280f);
glm::vec3 sunDir = glm::vec3(-0.93633f, 0.351106, 0.003226f);
glm::vec3 sunColor = glm::vec3(0.9f, 0.9f, 0.7f)*5;

glm::vec3 cameraPos = glm::vec3(0.479490f, 1.250000f, -2.124680f);
glm::vec3 cameraDir = glm::vec3(-0.354510f, 0.000000f, 0.935054f);


glm::vec3 spaceshipPos = glm::vec3(0.065808f, 1.250000f, -2.189549f);
glm::vec3 spaceshipDir = glm::vec3(-0.490263f, 0.000000f, 0.871578f);
GLuint VAO, VBO;

float aspectRatio = 1.f;

float exposition = 1.f;

glm::vec3 pointlightPos = glm::vec3(0, 2, 0);
glm::vec3 pointlightColor = glm::vec3(0.9, 0.6, 0.6);

bool flashlight = false;
bool christmasLightsOn = true;
int lightsIdx = 7;
int lightningMode = 2;
int intTime = 0;

std::vector<Triplet> lastTenPositions = { { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f },
										   { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f } };

glm::mat4 lightVP = glm::ortho(-3.0f, 5.0f, -5.0f, 10.0f, 1.0f, 12.0f) * glm::lookAt(sunPos, sunPos - sunDir, glm::vec3(0, 1, 0));
glm::mat4 sleighLightVP;

glm::vec3 chrismas_balls_pos[] = {
	glm::vec3(-0.95f, 0.45f, 1.45f),
	glm::vec3(-0.75f, 0.7f, 1.85f),
	glm::vec3(-1.15f, 1.0f, 1.52f),
	glm::vec3(-0.9f, 1.23f, 1.8f),
	glm::vec3(-1.2f, 1.55f, 1.65f),
	glm::vec3(-1.03f, 1.8f, 1.85f),
	glm::vec3(-1.18f, 2.27f, 1.83f),
};

glm::vec3 pointLightPositions_arr[8] = {
	pointlightPos,
	chrismas_balls_pos[0],
	chrismas_balls_pos[1],
	chrismas_balls_pos[2],
	chrismas_balls_pos[3],
	chrismas_balls_pos[4],
	chrismas_balls_pos[5],
	glm::vec3(-1.1f, 2.22f, 1.71f),
	//-0.05f, -0.02f
};

glm::vec3 pointlightColor_arr[8] = {
	glm::vec3(0.9, 0.6, 0.6),
	glm::vec3(1.0, 0.0, 0.0),
	glm::vec3(1.0, 1.0, 0.0),
	glm::vec3(0.0, 0.0, 1.0),
	glm::vec3(1.0, 0.0, 0.0),
	glm::vec3(1.0, 1.0, 0.0),
	glm::vec3(0.0, 0.0, 1.0),
	glm::vec3(1.0, 1.0, 0.0),
};

glm::vec3 original_pointlightColor_arr[8] = {
	glm::vec3(0.9, 0.6, 0.6),
	glm::vec3(1.0, 0.0, 0.0),
	glm::vec3(1.0, 1.0, 0.0),
	glm::vec3(0.0, 0.0, 1.0),
	glm::vec3(1.0, 0.0, 0.0),
	glm::vec3(1.0, 1.0, 0.0),
	glm::vec3(0.0, 0.0, 1.0),
	glm::vec3(1.0, 1.0, 0.0),
};

glm::vec3 spotlightPos = glm::vec3(0, 0, 0);
glm::vec3 spotlightConeDir = glm::vec3(0, 0, 0);
glm::vec3 spotlightColor = glm::vec3(0.4, 0.4, 0.9)*3;
float spotlightPhi = 3.14 / 4;


float lastTime = -1.f;
float deltaTime = 0.f;

void updateDeltaTime(float time) {
	if (lastTime < 0) {
		lastTime = time;
		return;
	}

	deltaTime = time - lastTime;
	if (deltaTime > 0.1) deltaTime = 0.1;
	lastTime = time;
}
glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir,glm::vec3(0.f,1.f,0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide,cameraDir));
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});
	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);
	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix()
{
	
	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 20.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,aspectRatio,0.,0.,
		0.,0.,(f+n) / (n - f),2*f * n / (n - f),
		0.,0.,-1.,0.,
		});

	
	perspectiveMatrix=glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectPBR(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, float alpha, float roughness, float metallic, glm::mat4 sleighLightVP) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(program, "exposition"), exposition);

	glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
	glUniform1f(glGetUniformLocation(program, "metallic"), metallic);

	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);
	glUniform1f(glGetUniformLocation(program, "alpha"), alpha);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(program, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(program, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

	glUniform3fv(glGetUniformLocation(program, "lightPos"), 8, (float*)&pointLightPositions_arr);
	glUniform3fv(glGetUniformLocation(program, "lightColor"), 8, (float*)&pointlightColor_arr);

	glUniform3f(glGetUniformLocation(program, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	
	if (flashlight) {
		glUniform3f(glGetUniformLocation(program, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	}
	else {
		glUniform3f(glGetUniformLocation(program, "spotlightColor"), 0.f, 0.f, 0.f);
	}
	
	glUniform1f(glGetUniformLocation(program, "spotlightPhi"), spotlightPhi);

	// SHADOW

	glActiveTexture(GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(program, "depthMap"), 0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniformMatrix4fv(glGetUniformLocation(program, "LightVP"), 1, GL_FALSE, (float*)&lightVP);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(program, "sleighDepthMap"), 1);
	glBindTexture(GL_TEXTURE_2D, sleighDepthMap);
	glUniformMatrix4fv(glGetUniformLocation(program, "sleighLightVP"), 1, GL_FALSE, (float*)&sleighLightVP);

	Core::DrawContext(context);

}

void drawObjectTexture(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureId, glm::mat4 sleighLightVP) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programTex, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	// glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(programTex, "depthMap"), 0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "LightVP"), 1, GL_FALSE, (float*)&lightVP);
	
	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(programTex, "sleighDepthMap"), 1);
	glBindTexture(GL_TEXTURE_2D, sleighDepthMap);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "sleighLightVP"), 1, GL_FALSE, (float*)&sleighLightVP);

	Core::SetActiveTexture(textureId, "colorTexture", programTex, 0);
	glUniform3f(glGetUniformLocation(programTex, "lightPos"), 0, 0, 0);

	Core::DrawContext(context);

}


void drawObjectPBRTexture(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint texture, float roughness, float metallic, float alpha, glm::mat4 sleighLightVP) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programTex, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(programTex, "depthMap"), 0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "LightVP"), 1, GL_FALSE, (float*)&lightVP);

	glUniform1f(glGetUniformLocation(programTex, "exposition"), exposition);

	glUniform1f(glGetUniformLocation(programTex, "roughness"), roughness);
	glUniform1f(glGetUniformLocation(programTex, "metallic"), metallic);
	glUniform1f(glGetUniformLocation(program, "alpha"), alpha);
	
	// glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(programTex, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(programTex, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(programTex, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

	glUniform3fv(glGetUniformLocation(programTex, "lightPos"), 8, (float*)&pointLightPositions_arr);
	glUniform3fv(glGetUniformLocation(programTex, "lightColor"), 8, (float*)&pointlightColor_arr);

	glUniform3f(glGetUniformLocation(programTex, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);

	glUniform3f(glGetUniformLocation(programTex, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);

	if (flashlight) {
		glUniform3f(glGetUniformLocation(programTex, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	}
	else {
		glUniform3f(glGetUniformLocation(programTex, "spotlightColor"), 0.f, 0.f, 0.f);
	}

	glUniform1f(glGetUniformLocation(programTex, "spotlightPhi"), spotlightPhi);

	
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(programTex, "depthMap"), 0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "LightVP"), 1, GL_FALSE, (float*)&lightVP);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(programTex, "sleighDepthMap"), 1);
	glBindTexture(GL_TEXTURE_2D, sleighDepthMap);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "sleighLightVP"), 1, GL_FALSE, (float*)&sleighLightVP);

	Core::SetActiveTexture(texture, "colorTexture", programTex, 2);

	Core::DrawContext(context);
}

void drawObjectPBRTextureNormal(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint texture, GLuint normal, float roughness, float metallic, glm::mat4 sleighLightVP) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programTexNormal, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programTexNormal, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(programTexNormal, "exposition"), exposition);

	glUniform1f(glGetUniformLocation(programTexNormal, "roughness"), roughness);
	glUniform1f(glGetUniformLocation(programTexNormal, "metallic"), metallic);

	Core::SetActiveTexture(texture, "colorTexture", programTexNormal, 2);
	Core::SetActiveTexture(normal, "normalSampler", programTexNormal, 3);
	// glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(programTexNormal, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(programTexNormal, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(programTexNormal, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

	glUniform3fv(glGetUniformLocation(programTexNormal, "lightPos"), 8, (float*)&pointLightPositions_arr);
	glUniform3fv(glGetUniformLocation(programTexNormal, "lightColor"), 8, (float*)&pointlightColor_arr);

	glUniform3f(glGetUniformLocation(programTexNormal, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);

	glUniform3f(glGetUniformLocation(programTexNormal, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	if (flashlight) {
		glUniform3f(glGetUniformLocation(programTexNormal, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	}
	else {
		glUniform3f(glGetUniformLocation(programTexNormal, "spotlightColor"), 0.f, 0.f, 0.f);
	}

	glUniform1f(glGetUniformLocation(programTexNormal, "spotlightPhi"), spotlightPhi);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(programTexNormal, "depthMap"), 0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniformMatrix4fv(glGetUniformLocation(programTexNormal, "LightVP"), 1, GL_FALSE, (float*)&lightVP);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(programTexNormal, "sleighDepthMap"), 1);
	glBindTexture(GL_TEXTURE_2D, sleighDepthMap);
	glUniformMatrix4fv(glGetUniformLocation(programTexNormal, "sleighLightVP"), 1, GL_FALSE, (float*)&sleighLightVP);

	Core::DrawContext(context);
}

void changeLights(float time) {
	if (glm::tan(time) < 0) { // wylaczamy swiatelka
		for (int i = 0; i < sizeof(chrismas_balls_pos) / sizeof(glm::vec3); i++)
		{
			pointlightColor_arr[i + 1] = glm::vec3(0.f, 0.f, 0.f);
		}
		christmasLightsOn = false;

	}
	else { // wlaczamy swiatelka
		for (int i = 0; i < sizeof(chrismas_balls_pos) / sizeof(glm::vec3); i++)
		{
			pointlightColor_arr[i + 1] = original_pointlightColor_arr[i + 1];
		}
		christmasLightsOn = true;
	}

}


void changeLights_2(float time) {
	if (intTime % 100 == 0 && christmasLightsOn) {
		pointlightColor_arr[lightsIdx] = glm::vec3(0.f, 0.f, 0.f);
		lightsIdx -= 1;
		if (lightsIdx == 0) {
			christmasLightsOn = false;
			lightsIdx = 1;
		}
	}
	else if (intTime % 100 == 0 && !christmasLightsOn) {
		pointlightColor_arr[lightsIdx] = original_pointlightColor_arr[lightsIdx];
		lightsIdx += 1;
		if (lightsIdx == 8) {
			christmasLightsOn = true;
			lightsIdx = 7;
		}
	}
}


void runLights(int lightningMode, float time) {
	if (lightningMode == 1) {
		changeLights(time);
	}
	else if (lightningMode == 2) {
		changeLights_2(time);
	}
}

void loadCubemap(char* textures[]) {
	glGenTextures(1, &texture::cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture::cubemapTexture);

	int w, h;
	unsigned char* data{};
	for (unsigned int i = 0; i < 6; i++)
	{
		data = SOIL_load_image(textures[i], &w, &h, 0, SOIL_LOAD_RGBA);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void drawSkybox(glm::mat4 modelMatrix) {
	glDisable(GL_DEPTH_TEST);

	glUseProgram(programSkybox);

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::SetActiveTexture(texture::cubemapTexture, "skybox", programSkybox, 0);

	Core::DrawContext(skyboxContext);

	glEnable(GL_DEPTH_TEST);
}

void initDepthMap() {
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initDepthMapSleigh() {
	glGenFramebuffers(1, &sleighDepthMapFBO);

	glGenTextures(1, &sleighDepthMap);
	glBindTexture(GL_TEXTURE_2D, sleighDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, sleighDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sleighDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void drawObjectDepth(Core::RenderContext& context, glm::mat4 viewProjection, glm::mat4 modelMatrix)
{
	glUniformMatrix4fv(glGetUniformLocation(programDepth, "viewProjectionMatrix"), 1, GL_FALSE, (float*)&viewProjection);
	glUniformMatrix4fv(glGetUniformLocation(programDepth, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	Core::DrawContext(context);

}

void renderShadow() {
	float time = glfwGetTime();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//uzupelnij o renderowanie glebokosci do tekstury
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(programDepth);


	drawObjectDepth(sphereContext, lightVP, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, -4.f, 0)) * glm::scale(glm::vec3(0.3f)));

	drawObjectDepth(sphereContext, lightVP, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, -4.f, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(0.1f)));

	drawObjectDepth(models::bedContext, lightVP, glm::mat4());
	drawObjectDepth(models::chairContext, lightVP, glm::mat4());
	drawObjectDepth(models::doorContext, lightVP, glm::mat4());
	drawObjectDepth(models::drawerContext, lightVP, glm::mat4());
	drawObjectDepth(models::marbleBustContext, lightVP, glm::mat4());
	drawObjectDepth(models::table2Context, lightVP, glm::mat4());
	drawObjectDepth(models::materaceContext, lightVP, glm::mat4());
	drawObjectDepth(models::pencilsContext, lightVP, glm::mat4());
	drawObjectDepth(models::planeContext, lightVP, glm::mat4());
	drawObjectDepth(models::windowContext, lightVP, glm::mat4());
	drawObjectDepth(models::roomContext, lightVP, glm::mat4());
	//drawObjectDepth(models::christmasPresent, lightVP, glm::mat4());
	//drawObjectDepth(models::christmasPresent2, lightVP, glm::mat4());
	//drawObjectDepth(models::christmasPresent4, lightVP, glm::mat4());
	//drawObjectDepth(models::monitor, lightVP, glm::mat4());
	//drawObjectDepth(models::keyboard, lightVP, glm::mat4());

	//drawObjectDepth(models::treeContext, lightVP, glm::mat4());
	//drawObjectDepth(models::treeTrunkContext, lightVP, glm::mat4());
	//drawObjectDepth(models::starContext, lightVP, glm::mat4());


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}

void renderShadowSleigh() {
	float time = glfwGetTime();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, sleighDepthMapFBO); 
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(programDepth);

	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 20.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		0.2,0.,0.,0.,
		0., 0.2,0.,0.,
		0.,0.,(f + n) / (n - f),2 * f * n / (n - f),
		0.,0.,-1.,0.,
		});


	perspectiveMatrix = glm::transpose(perspectiveMatrix);
	sleighLightVP = perspectiveMatrix * glm::lookAt(spotlightPos, spotlightPos + spotlightConeDir, glm::vec3(0, 1, 0));

	drawObjectDepth(sphereContext, sleighLightVP, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time * 2) * glm::translate(glm::vec3(4.f, 0.f, 0)) * glm::scale(glm::vec3(0.3f)));

	drawObjectDepth(sphereContext, sleighLightVP, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time * 2) * glm::translate(glm::vec3(4.f, 0.f, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(0.1f)));

	drawObjectDepth(models::bedContext, sleighLightVP, glm::mat4());
	drawObjectDepth(models::chairContext, sleighLightVP, glm::mat4());
	drawObjectDepth(models::doorContext, sleighLightVP, glm::mat4());
	drawObjectDepth(models::drawerContext, sleighLightVP, glm::mat4());
	drawObjectDepth(models::marbleBustContext, sleighLightVP, glm::mat4());
	drawObjectDepth(models::table2Context, sleighLightVP, glm::mat4());
	drawObjectDepth(models::materaceContext, sleighLightVP, glm::mat4());
	drawObjectDepth(models::pencilsContext, sleighLightVP, glm::mat4());
	drawObjectDepth(models::planeContext, sleighLightVP, glm::mat4());
	drawObjectDepth(models::windowContext, sleighLightVP, glm::mat4());
	drawObjectDepth(models::roomContext, sleighLightVP, glm::mat4());
	//drawObjectDepth(models::christmasPresent, sleighLightVP, glm::mat4());
	//drawObjectDepth(models::christmasPresent2, sleighLightVP, glm::mat4());
	//drawObjectDepth(models::christmasPresent4, sleighLightVP, glm::mat4());
	//drawObjectDepth(models::monitor, sleighLightVP, glm::mat4());
	//drawObjectDepth(models::keyboard, sleighLightVP, glm::mat4());

	//drawObjectDepth(models::treeContext, sleighLightVP, glm::mat4());
	//drawObjectDepth(models::treeTrunkContext, sleighLightVP, glm::mat4());
	//drawObjectDepth(models::starContext, sleighLightVP, glm::mat4());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}

void renderScene(GLFWwindow* window)
{
	glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float time = glfwGetTime();
	intTime += 1;
	updateDeltaTime(time);

	renderShadow();
	renderShadowSleigh();

	drawSkybox(glm::translate(cameraPos));

	//space lamp
	glUseProgram(programSun);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1));
	glUniformMatrix4fv(glGetUniformLocation(programSun, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform3f(glGetUniformLocation(programSun, "color"), sunColor.x / 2, sunColor.y / 2, sunColor.z / 2);
	glUniform1f(glGetUniformLocation(programSun, "exposition"), exposition);
	Core::DrawContext(sphereContext);

	glUseProgram(program);

	glUseProgram(programTex);
	drawObjectPBRTexture(sphereContext, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time * 2) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::scale(glm::vec3(0.4f)), texture::earth, 0.3, 0.0, 1.0, sleighLightVP);

	for (int i = 0; i < 10; i++) {
		drawObjectPBRTexture(sphereContext, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time * 2) * glm::translate(glm::vec3(4.f, 0, 0.1 * i)) * glm::scale(glm::vec3(0.4f)), texture::earth, 0.3, 0.0, 0.5 - 0.05 * i, sleighLightVP);
	}

	drawObjectPBRTexture(sphereContext,
		glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time * 2) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(0.2f)),
		texture::moon, 0.3, 0.0, 1.0, sleighLightVP);

	for (int i = 0; i < 5; i++) {
		//drawObjectPBRTexture(sphereContext, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time * 2) * glm::translate(glm::vec3(4.f, 0, 0.1 * i)) * glm::scale(glm::vec3(0.4f)), texture::earth, 0.3, 0.0, 0.5 - 0.05 * i, sleighLightVP);
		drawObjectPBRTexture(sphereContext,
			glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time * 2) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, 0.04 * i)) * glm::scale(glm::vec3(0.2f)),
			texture::moon, 0.3, 0.0, 0.5 - 0.1 * i, sleighLightVP);
		drawObjectPBRTexture(sphereContext,
			glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time * 2) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, -0.04 * i)) * glm::scale(glm::vec3(0.2f)),
			texture::moon, 0.3, 0.0, 0.5 - 0.1 * i, sleighLightVP);
	}

	glUseProgram(program);

	drawObjectPBR(models::bedContext, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 1.0, 0.2f, 0.0f, sleighLightVP);
	drawObjectPBR(models::chairContext, glm::mat4() * glm::translate(glm::vec3(-0.1f, 0.f, -0.1f)), glm::vec3(0.195239f, 0.37728f, 0.8f), 1.0, 0.4f, 0.0f, sleighLightVP);
	//drawObjectPBR(models::deskContext, glm::mat4(), glm::vec3(0.428691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::doorContext, glm::mat4() * glm::scale(glm::vec3(1.f, 1.f, 1.f)), glm::vec3(0.402978f, 0.120509f, 0.057729f), 1.0, 0.2f, 0.0f, sleighLightVP);
	drawObjectPBR(models::drawerContext, glm::mat4(), glm::vec3(0.428691f, 0.08022f, 0.036889f), 1.0, 0.2f, 0.0f, sleighLightVP);

	glUseProgram(programTex);
	//drawObjectTexture(models::marbleBustContext, glm::mat4(), texture::earth);
	drawObjectPBRTexture(models::marbleBustContext, glm::mat4() * glm::translate(glm::vec3(0.f, -0.15f, 0.f)), texture::earth, 1.f, 0.5f, 1.f, sleighLightVP);
	glUseProgram(program);
	
	glUseProgram(programTex);
	drawObjectPBRTexture(models::table2Context, glm::mat4() * glm::translate(glm::vec3(-1.4f, 0.9f, 0.f)) * glm::scale(glm::vec3(0.85f, 1.0f, 0.85f)) * 
		glm::rotate(glm::mat4(), glm::radians(-90.f), glm::vec3(1, 0, 0)) *
		glm::rotate(glm::mat4(), glm::radians(90.f), glm::vec3(0, 0, 1)), texture::tableWood, 0.2f, 0.0f, 1.f, sleighLightVP);
	glUseProgram(program);

	// prezenty
	glUseProgram(programTex);
	drawObjectPBRTexture(models::christmasPresent, glm::mat4() * glm::translate(glm::vec3(-0.8f, 0.0f, 1.5f)) * glm::scale(glm::vec3(0.02f))
		* glm::rotate(glm::mat4(), glm::radians(-45.f), glm::vec3(0, 1, 0)), texture::present, 0.5f, 1.f, 1.f, sleighLightVP);
	drawObjectPBRTexture(models::christmasPresent2, glm::mat4() * glm::translate(glm::vec3(-1.3f, 0.0f, 1.3f)) * glm::scale(glm::vec3(0.15f)),
		texture::present2, 0.5f, 1.f, 1.f, sleighLightVP);
	drawObjectPBRTexture(models::christmasPresent4, glm::mat4() * glm::rotate(glm::mat4(), glm::radians(-90.f), glm::vec3(1, 0, 0)) * glm::translate(glm::vec3(-0.6f, -2.0f, 0.f))
		* glm::scale(glm::vec3(0.015f)), texture::present4, 0.5f, 1.f, 1.f, sleighLightVP);

	// monitor
	drawObjectPBRTexture(models::monitor, glm::mat4() * glm::translate(glm::vec3(-1.45f, 1.18f, -0.6f)) * glm::rotate(glm::mat4(), glm::radians(45.f), glm::vec3(0, 1, 0))
		* glm::scale(glm::vec3(0.01)), texture::monitor, 0.5f, 0.75f, 1.f, sleighLightVP);

	// poduszka
	drawObjectPBRTexture(models::pillow, glm::mat4() * glm::translate(glm::vec3(0.9f, 0.6f, 0.65f)) * glm::rotate(glm::mat4(), glm::radians(90.f), glm::vec3(0, 1, 0))
		* glm::scale(glm::vec3(0.0036)), texture::pillow, 2.5f, 0.75f, 1.f, sleighLightVP);

	// dywan
	drawObjectPBRTexture(models::carpet, glm::mat4() * glm::translate(glm::vec3(-0.2f, 0.1f, -0.25f)) * glm::rotate(glm::mat4(), glm::radians(90.f), glm::vec3(1, 0, 0))
		* glm::rotate(glm::mat4(), glm::radians(90.f), glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(0.01)), texture::carpet, 2.5f, 0.75f, 1.f, sleighLightVP);

	// klawiaturka
	drawObjectPBRTexture(models::keyboard, glm::mat4() * glm::translate(glm::vec3(-1.25f, 0.93f, -0.4f)) * glm::rotate(glm::mat4(), glm::radians(45.f), glm::vec3(0, 1, 0))
		* glm::rotate(glm::mat4(), glm::radians(-90.f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(0.008)), texture::keyboard, 0.5f, 1.f, 1.f, sleighLightVP);

	// podloga
	drawObjectPBRTexture(models::planeContext, glm::mat4() * glm::scale(glm::vec3(1.1f)), texture::tableWood, 0.3f, 0.0f, 1.f, sleighLightVP);


	//podloga
	//drawObjectPBR(models::planeContext, glm::mat4(), glm::vec3(0.402978f, 0.120509f, 0.057729f), 0.2f, 0.0f, sleighLightVP);
	
	glUseProgram(program);
	drawObjectPBR(models::materaceContext, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), 1.0, 0.8f, 0.0f, sleighLightVP);
	drawObjectPBR(models::pencilsContext, glm::mat4() * glm::translate(glm::vec3(0.f, -0.1f, 0.f)), glm::vec3(0.10039f, 0.018356f, 0.001935f), 1.0, 0.1f, 0.0f, sleighLightVP);

	// prolly trzeba bedzie powiekszysc pokój, wiec tu zamiast skalowac 1.1f, to trzeba bedzie np. (1.5f, 1.f, 1.f)
	//drawObjectPBR(models::planeContext, glm::mat4() * glm::scale(glm::vec3(1.1f)), glm::vec3(0.402978f, 0.120509f, 0.057729f), 0.2f, 0.0f, sleighLightVP);
	drawObjectPBR(models::roomContext, glm::mat4() * glm::scale(glm::vec3(1.f, 1.0f, 1.f)), glm::vec3(0.9f, 0.9f, 0.9f), 1.0, 0.8f, 0.0f, sleighLightVP);
	drawObjectPBR(models::windowContext, glm::mat4() * glm::scale(glm::vec3(1.f, 1.f, 1.f)), glm::vec3(0.402978f, 0.120509f, 0.057729f), 1.0, 0.2f, 0.0f, sleighLightVP);
	
	// christmas tree
	drawObjectPBR(models::treeContext, glm::mat4() * glm::translate(glm::vec3(-1.2f, 0.7f, 1.85f)) * glm::scale(glm::vec3(0.5f)), glm::vec3(0.f, 1.f, 0.f), 1.0, 0.2f, 0.0f, sleighLightVP);
	drawObjectPBR(models::treeTrunkContext, glm::mat4() * glm::translate(glm::vec3(-1.2f, 0.0f, 1.85f)) * glm::scale(glm::vec3(0.5f)), glm::vec3(0.19, 0.11, 0.02), 1.0, 0.2f, 0.0f, sleighLightVP);
	
	// christmas balls
	drawObjectPBR(sphereContext, glm::translate(chrismas_balls_pos[0]) * glm::scale(glm::vec3(0.07)), glm::vec3(1.0, 0.0, 0.0), 1.0, 0.5, 0.0, sleighLightVP);
	drawObjectPBR(sphereContext, glm::translate(chrismas_balls_pos[1]) * glm::scale(glm::vec3(0.07)), glm::vec3(1.0, 1.0, 0.0), 1.0, 0.5, 0.0, sleighLightVP);
	drawObjectPBR(sphereContext, glm::translate(chrismas_balls_pos[2]) * glm::scale(glm::vec3(0.07)), glm::vec3(0.0, 0.0, 1.0), 1.0, 0.5, 0.0, sleighLightVP);
	drawObjectPBR(sphereContext, glm::translate(chrismas_balls_pos[3]) * glm::scale(glm::vec3(0.07)), glm::vec3(1.0, 0.0, 0.0), 1.0, 0.5, 0.0, sleighLightVP);
	drawObjectPBR(sphereContext, glm::translate(chrismas_balls_pos[4]) * glm::scale(glm::vec3(0.07)), glm::vec3(1.0, 1.0, 0.0), 1.0, 0.5, 0.0, sleighLightVP);
	drawObjectPBR(sphereContext, glm::translate(chrismas_balls_pos[5]) * glm::scale(glm::vec3(0.07)), glm::vec3(0.0, 0.0, 1.0), 1.0, 0.5, 0.0, sleighLightVP);

	// christmas star
	drawObjectPBR(models::starContext, glm::mat4() * glm::translate(chrismas_balls_pos[6]) * glm::translate(glm::vec3(0.025f, -0.05f, -0.02f)) * glm::scale(glm::vec3(0.1)) *
		glm::rotate(glm::mat4(), glm::radians(55.f), glm::vec3(0, 1, 0)) *
		glm::rotate(glm::mat4(), glm::degrees(45.f), glm::vec3(1, 0, 0)), glm::vec3(1.f, 1.f, 0.f), 1.0, 0.5f, 0.0f, sleighLightVP);

	// change lights 
	runLights(lightningMode, time);
	// std::cout << intTime << std::endl;

	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});

	// flashlight
	spotlightPos = spaceshipPos + 0.2 * spaceshipDir;
	spotlightConeDir = spaceshipDir;

	//drawObjectColor(shipContext,
	//	glm::translate(cameraPos + 1.5 * cameraDir + cameraUp * -0.5f) * inveseCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()),
	//	glm::vec3(0.3, 0.3, 0.5)
	//	);
	drawObjectPBR(shipContext,
		glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.03f)),
		glm::vec3(1.0, 0.1, 0.1), 1.0,
		0.2, 0.5, sleighLightVP);

	//test depth buffer
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUseProgram(programTest);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	//Core::DrawContext(models::testContext);

	glUseProgram(0);
	glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
}
void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_CULL_FACE);
	//glEnable(GL_CW);

	program = shaderLoader.CreateProgram("shaders/shader_9_1.vert", "shaders/shader_9_1.frag");
	programTest = shaderLoader.CreateProgram("shaders/test.vert", "shaders/test.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_8_sun.vert", "shaders/shader_8_sun.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox-1.vert", "shaders/shader_skybox-1.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programDepth = shaderLoader.CreateProgram("shaders/shader_shadow.vert", "shaders/shader_shadow.frag");
	programTexNormal = shaderLoader.CreateProgram("shaders/shader_tex_normal.vert", "shaders/shader_tex_normal.frag");

	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/santa_sliegh.fbx", shipContext);
	loadModelToContext("./models/cube.obj", skyboxContext);


	loadModelToContext("./models/bed.obj", models::bedContext);
	loadModelToContext("./models/chair.obj", models::chairContext);
	loadModelToContext("./models/desk.obj", models::deskContext);
	loadModelToContext("./models/door.obj", models::doorContext);
	loadModelToContext("./models/drawer.obj", models::drawerContext);
	loadModelToContext("./models/marbleBust.obj", models::marbleBustContext);
	loadModelToContext("./models/materace.obj", models::materaceContext);
	loadModelToContext("./models/pencils.obj", models::pencilsContext);
	loadModelToContext("./models/plane.obj", models::planeContext);
	loadModelToContext("./models/room.obj", models::roomContext);
	loadModelToContext("./models/santa_sliegh.fbx", models::spaceshipContext);
	loadModelToContext("./models/sphere.obj", models::sphereContext);
	loadModelToContext("./models/window.obj", models::windowContext);
	loadModelToContext("./models/test.obj", models::testContext);
	loadModelToContext("./models/lowpolytree.obj", models::treeContext);
	loadModelToContext("./models/lowpolytrunk.obj", models::treeTrunkContext);
	loadModelToContext("./models/christmas.fbx", models::starContext);
	loadModelToContext("./models/cube.obj", skyboxContext);
	loadModelToContext("./models/table_download.fbx", models::table2Context);
	loadModelToContext("./models/giftTest.fbx", models::christmasPresent);
	loadModelToContext("./models/BoxHappy.fbx", models::christmasPresent2);
	loadModelToContext("./models/pr_present_uv.fbx", models::christmasPresent3);
	loadModelToContext("./models/Gift.fbx", models::christmasPresent4);
	loadModelToContext("./models/Monitor.fbx", models::monitor);
	loadModelToContext("./models/keyboard.obj", models::keyboard);
	loadModelToContext("./models/pillow.obj", models::pillow);
	loadModelToContext("./models/carpet.obj", models::carpet);

	char* textures[] = { "textures/skybox/sh_ft.png", "textures/skybox/sh_bk.png", "textures/skybox/sh_up.png", "textures/skybox/sh_dn.png", "textures/skybox/sh_rt.png", "textures/skybox/sh_lf.png" };
	loadCubemap(textures);

	texture::earth = Core::LoadTexture("./textures/earth.png");
	texture::tableWood = Core::LoadTexture("./textures/table_wood.jpg");
	texture::sleigh = Core::LoadTexture("./textures/sleigh.jpg");
	texture::present = Core::LoadTexture("./textures/giftTex.png");
	texture::present2 = Core::LoadTexture("./textures/lambert1_albedo.jpeg");
	texture::present3 = Core::LoadTexture("./textures/paket_ny_albedo_F.png");
	texture::present4 = Core::LoadTexture("./textures/present.png");
	texture::monitor = Core::LoadTexture("./textures/monitor.jpg");
	texture::keyboard = Core::LoadTexture("./textures/keyboard.jpg");
	texture::moon = Core::LoadTexture("./textures/moon.jpg");
	texture::pillow = Core::LoadTexture("./textures/pillow.png");
	texture::carpet = Core::LoadTexture("./textures/carpet.jpg");

	normals::normal_present2 = Core::LoadTexture("./textures/lambert1_normal.png");
	normals::normal_present3 = Core::LoadTexture("./textures/paket_ny_normal.png");


	initDepthMap();
	initDepthMapSleigh();
}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f,1.f,0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);
	float angleSpeed = 0.05f * deltaTime * 60;
	float moveSpeed = 0.05f * deltaTime * 60;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		spaceshipPos += spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		spaceshipPos -= spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		spaceshipPos += spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		spaceshipPos -= spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		spaceshipPos += spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		spaceshipPos -= spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(spaceshipDir, 0));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(spaceshipDir, 0));
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		spaceshipPos += spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		spaceshipPos -= spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleZ(angleSpeed) * glm::vec4(spaceshipDir, 0));
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleZ(-angleSpeed) * glm::vec4(spaceshipDir, 0));

	cameraPos = spaceshipPos - 0.5 * spaceshipDir + glm::vec3(0, 1, 0) * 0.2f;
	cameraDir = spaceshipDir;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		exposition -= 0.05;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		exposition += 0.05;

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		printf("spaceshipPos = glm::vec3(%ff, %ff, %ff);\n", spaceshipPos.x, spaceshipPos.y, spaceshipPos.z);
		printf("spaceshipDir = glm::vec3(%ff, %ff, %ff);\n", spaceshipDir.x, spaceshipDir.y, spaceshipDir.z);
	}

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {

		lightningMode = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
		for (int i = 0; i < sizeof(chrismas_balls_pos) / sizeof(glm::vec3); i++)
		{
			pointlightColor_arr[i + 1] = original_pointlightColor_arr[i + 1];
		}
		christmasLightsOn = true;
		lightningMode = 2;
		lightsIdx = 7;
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
		flashlight = !flashlight;
		Sleep(100);
	}

	//cameraDir = glm::normalize(-cameraPos);
}

// funkcja jest glowna petla
void renderLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		glfwPollEvents();
	}
}
//}