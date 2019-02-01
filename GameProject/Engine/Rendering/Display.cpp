#include "Display.h"

#include "../Config.h"
#include "../../Utils/Logger.h"
#include "../Events/EventBus.h"
#include "../Events/Events.h"

Display & Display::get()
{
	static bool isFirst = true;
	static Display display;
	if (isFirst) {
		display.init(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_TITLE);
		isFirst = false;
	}
	return display;
}

void Display::updateView(int width, int height)
{
	this->width = width;
	this->height = height;
	glViewport(0, 0, this->width, this->height);
}

bool Display::isOpen() const
{
	return glfwGetKey(this->window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(this->window);
}

void Display::startFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display::endFrame()
{
	glfwSwapBuffers(this->window);
	glfwPollEvents();
}

void Display::setTitle(const std::string & title)
{
	this->title = title;
	glfwSetWindowTitle(this->window, title.c_str());
}

void Display::setTitleSuffix(const std::string & suffix)
{
	glfwSetWindowTitle(this->window, (this->title + suffix).c_str());
}

int Display::getWidth() const
{
	return this->width;
}

int Display::getHeight() const
{
	return this->height;
}

float Display::getRatio() const
{
	return (float)this->width / (float)this->height;
}

float Display::getPixelXScale() const
{
	return 1.0f / (float)this->width;
}

float Display::getPixelYScale() const
{
	return 1.0f / (float)this->height;
}

std::string Display::getTitle() const
{
	return this->title;
}

GLFWwindow * Display::getWindowPtr()
{
	return this->window;
}

Renderer & Display::getRenderer()
{
	return this->renderer;
}

Display::~Display()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();
}

void Display::errorCallback(int error, const char * description)
{
	LOG_ERROR("Glfw not initialized! %s", description);
}

void Display::resizeCallback(GLFWwindow * window, int width, int height)
{
	if (width != 0 || height != 0)
	{
		Display::get().updateView(width, height);
		EventBus::get().publish(&WindowResizeEvent(width, height));
	}
}

void Display::init(int width, int height, const std::string& title)
{
	this->width = width;
	this->height = height;
	this->title = title;

	if(!glfwInit()) 
	{
		LOG_ERROR("Failed to initialize GLFW!");
		exit(EXIT_FAILURE);
	}

	glfwSetErrorCallback(errorCallback);

	//glfwWindowHint(GLFW_SAMPLES, 4);// 4x antialiasing
	// OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Modern opengl

	this->window = glfwCreateWindow(this->width, this->height, this->title.c_str(), NULL, NULL);
	if (this->window == NULL)
	{
		LOG_ERROR("Failed to create GLFW window!");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(this->window);
	glfwSwapInterval(1); // Enable vsync

	// --------------- INIT GLEW ---------------
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK)
	{
		LOG_ERROR("Failed to initialize GLEW!");
		exit(EXIT_FAILURE);
	}

	glViewport(0, 0, this->width, this->height);

	glfwSetWindowSizeCallback(this->window, resizeCallback);
}
