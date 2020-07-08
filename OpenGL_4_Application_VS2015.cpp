//
//  main.cpp
//  OpenGL_Lighting_Example_step1
//
//  Created by CGIS on 21/11/16.
//  Copyright © 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#include "stb_image.h"
#include "SkyBox.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "Model3D.hpp"
#include "Mesh.hpp"

int glWindowWidth = 640;
int glWindowHeight = 480;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;
glm::vec3 viewPos;
GLuint viewPosLoc;

GLuint viewDirLoc;

//lights
GLint pointlightToggleLoc;
GLint dirLightToggleLoc;

bool lightOn = false;
bool dirLightOn = true;
bool fogOn = false;

gps::Camera cam(glm::vec3(2.0f, 5.5f, 20.0f), 3.0f, 10.0f);

bool pressedKeys[1024];

GLfloat angle = 0.0f, cameraSpeed = 10.1f;
GLfloat rootDudeAngle = 0.0f;

gps::Shader myCustomShader;

glm::vec2 mouse, mousePrev, mouseDelta;

gps::Model3D rootDude;
gps::Model3D winter;

std::vector<const GLchar*> skyBoxImg;
gps::Shader skyBoxShader;
gps::SkyBox skyBox;

double currentTime = 0, lastTime = 0, deltaTime = 0;

int wireframeView = 0;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);
	glViewport(0, 0, retina_width, retina_height);

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void staticKeys()
{
	if (pressedKeys[GLFW_KEY_SPACE])
	{
		std::cout << "Position: " << cam.getPosition().x << " " << cam.getPosition().y << " " << cam.getPosition().z << std::endl;
		std::cout << "Rotation: " << cam.getRotation().x << " " << cam.getRotation().y << " " << cam.getRotation().z << std::endl;
	}

	if (pressedKeys[GLFW_KEY_O]) {
		//toggle light 1
		lightOn = !lightOn;
	}

	if (pressedKeys[GLFW_KEY_F]) {
		//toggle fog
		fogOn = !fogOn;
	}

	if (pressedKeys[GLFW_KEY_I]) {
		//toggle directional light
		dirLightOn = !dirLightOn;
	}

	if (pressedKeys[GLFW_KEY_G]) {
		wireframeView = !wireframeView;

		if(!wireframeView) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;

		if (key == GLFW_KEY_I || key == GLFW_KEY_O || key == GLFW_KEY_SPACE || key == GLFW_KEY_P || key == GLFW_KEY_G || key == GLFW_KEY_M || key == GLFW_KEY_F || key == GLFW_KEY_N)
		{
			staticKeys();
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	mouse.x = xpos;
	mouse.y = ypos;
}

void initObjects()
{
	winter = gps::Model3D("models/winter/winter.obj", "models/winter/");
	rootDude = gps::Model3D("models/root/source/pot_dude3.obj", "models/root/source/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	skyBoxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
}

void initskyBox()
{
	skyBoxImg.push_back("skybox/mp_blizzard/blizzard_rt.tga");
	skyBoxImg.push_back("skybox/mp_blizzard/blizzard_lf.tga");
	skyBoxImg.push_back("skybox/mp_blizzard/blizzard_up.tga");
	skyBoxImg.push_back("skybox/mp_blizzard/blizzard_dn.tga");
	skyBoxImg.push_back("skybox/mp_blizzard/blizzard_bk.tga");
	skyBoxImg.push_back("skybox/mp_blizzard/blizzard_ft.tga");

	skyBox.Load(skyBoxImg);
}

void drawSkyBox()
{
	skyBoxShader.useShaderProgram();
	skyBox.Draw(skyBoxShader, view, projection);
}

void initUniforms()
{
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");

	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	lightDir = glm::vec3(0.0f, -1.0f, -1.0f);

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction
	lightDir = glm::vec3(0.0f, -1.0f, -1.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	viewDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "viewDir");

	pointlightToggleLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightOn");
	dirLightToggleLoc = glGetUniformLocation(myCustomShader.shaderProgram, "dirLightOn");
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glEnable(GL_MULTISAMPLE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);


	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);

	return true;
}

void rotateLight()
{
	myCustomShader.useShaderProgram();
	lightDir = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(lightDir, 1.0f));
}

void initOpenGL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void processKeys()
{
	if (pressedKeys[GLFW_KEY_A]) {
		cam.rotate(-1.0f * deltaTime, 0.0f);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		cam.rotate(1.0f * deltaTime, 0.0f);
	}

	if (pressedKeys[GLFW_KEY_W]) {
		cam.move(gps::MOVE_FORWARD, deltaTime);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		cam.move(gps::MOVE_BACKWARD, deltaTime);
	}

	if (pressedKeys[GLFW_KEY_UP]) {
		cam.rotate(0.0f, 1.0f * deltaTime);
	}

	if (pressedKeys[GLFW_KEY_DOWN]) {
		cam.rotate(0.0f, -1.0f * deltaTime);
	}

	if (pressedKeys[GLFW_KEY_RIGHT]) {
		angle += 1.0f * deltaTime;

		if (angle > 3.08f) angle -= 3.08f;

		std::cout << "lightAngle: " << angle << std::endl;

		rotateLight();
	}

	if (pressedKeys[GLFW_KEY_LEFT]) {
		angle -= 1.0f * deltaTime;

		if (angle < 0.0f) angle += 3.08f;
		std::cout << "lightAngle: " << angle << std::endl;

		rotateLight();
	}
}

void rotateRootDude()
{
	rootDudeAngle += 10.0 * deltaTime;
	if (rootDudeAngle > 360.0f) rootDudeAngle -= 360.0f;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(4.5f, 0.0f, 0.0f));

	model = glm::rotate(model, glm::radians(rootDudeAngle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void renderScene()
{
	myCustomShader.useShaderProgram();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	processKeys();

	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	model = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = cam.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	glUniform3fv(viewDirLoc, 1, glm::value_ptr(cam.getRotation()));

	if (lightOn)
	{
		glUniform1i(pointlightToggleLoc, 1);
	}

	else
	{
		glUniform1i(pointlightToggleLoc, 0);
	}

	if (dirLightOn)
	{
		glUniform1i(dirLightToggleLoc, 1);
	}

	else
	{
		glUniform1i(dirLightToggleLoc, 0);
	}

	if (fogOn)
	{
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogOn"), 1);
	}

	else
	{
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogOn"), 0);
	}

	if (wireframeView)
	{
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "wireFrameOn"), 1);
	}

	else
	{
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "wireFrameOn"), 0);
	}

	
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "viewPos"), 1, glm::value_ptr(cam.getPosition()));

	winter.Draw(myCustomShader);

	rotateRootDude();
	rootDude.Draw(myCustomShader);

	drawSkyBox();
}

int main(int argc, const char * argv[]) 
{
	//glCheckError();
	initOpenGLWindow();
	initOpenGL();
	initskyBox();
	initObjects();
	initShaders();
	initUniforms();

	while (!glfwWindowShouldClose(glWindow)) 
	{
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;

		if (deltaTime >= 1.0f / 60.0f)
		{
			lastTime = currentTime;
		}

		renderScene();
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
