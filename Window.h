#pragma once

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{

public:
	Window();

	Window(GLint windowWidth, GLint windowHeight);

	int Initialise();

	GLfloat getBufferWidth();
	GLfloat getBufferHeight();

	bool getShouldClose();
	GLfloat getXChange();
	GLfloat getYChange();

	bool* getKeys();


	void SwapBuffers();
	
	~Window();

private:

	GLFWwindow* mainWindow;
	GLint width, height, bufferWidth, bufferHeight;

	bool keys[1024];

	GLfloat lastX;
	GLfloat lastY;
	GLfloat changeX;
	GLfloat changeY;
	bool mouseFirstMoved;

	void createCallbacks();
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);

};

