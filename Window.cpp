#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;

    for (size_t i = 0; i < 1024; i++)
    {
        keys[i] = 0;
    }

    changeX = 0.0f;
    changeY = 0.0f;
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

    for (size_t i = 0; i < 1024; i++) 
    {
        keys[i] = 0;
    }

    changeX = 0.0f;
    changeY = 0.0f;
}

int Window::Initialise()
{
    //Initialise GLFW
    if (!glfwInit())
    {
        printf("GLFW initialisation FAILED!");
        glfwTerminate();
        return 1;
    }

    //setup GLFW window properties
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
    if (!mainWindow)
    {
        printf("GLFW window creation FAILED!");
        glfwTerminate();
        return 1;
    }

    //Get Buffer Size information
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    glfwMakeContextCurrent(mainWindow);

    //handle input
    createCallbacks();
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Allow modern extenstion features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initialisation FAILED!");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    //Setup viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);

    glfwSetWindowUserPointer(mainWindow, this);
}

GLfloat Window::getBufferWidth()
{
	return bufferWidth;
}

GLfloat Window::getBufferHeight()
{
	return bufferHeight;
}

bool Window::getShouldClose()
{
	return glfwWindowShouldClose(mainWindow);
}

GLfloat Window::getXChange()
{
    GLfloat theChange = changeX;
    changeX = 0.0f;
    return theChange;
}

GLfloat Window::getYChange()
{
    GLfloat theChange = changeY;
    changeY = 0.0f;
    return theChange;
}

bool* Window::getKeys()
{
    return keys;
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(mainWindow);
}

Window::~Window()
{
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
}

void Window::createCallbacks()
{
    glfwSetKeyCallback(mainWindow, handleKeys);
    glfwSetCursorPosCallback(mainWindow, handleMouse);
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key <= 1024) //avoids issues with special characters
    {
        if (action == GLFW_PRESS)
        {
            theWindow->keys[key] = true;
            
        }
        else if (action == GLFW_RELEASE)
        {
            theWindow->keys[key] = false;
            
        }
    }
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
   
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (theWindow->mouseFirstMoved)
    {
        theWindow->lastX = xPos;
        theWindow->lastY = yPos;
        theWindow->mouseFirstMoved = false;
    }

    theWindow->changeX = xPos - theWindow->lastX;
    theWindow->changeY = theWindow->lastY - yPos; //avoids inverted camera

    theWindow->lastX = xPos;
    theWindow->lastY = yPos;
}
