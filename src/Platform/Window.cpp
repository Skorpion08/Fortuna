#include "Window.h"


#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "../Core/Log.h"

static void GLFWErrorCallback(int error, const char* description)
{
	Log::Error("GLFW Error {}: {}", error, description);
}

static void GLFWFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	static_cast<Window*>(glfwGetWindowUserPointer(window))->SetWindowSize(width, height);
}

Window::Window(int _width, int _height, const std::string& _title)
	: width(_width), height(_height), title(_title)
{
	Log::Info("Creating window '{}'...", GetTitle());

	Log::Info("Initializing glfw");
	if (!glfwInit())
	{
		const char* err = nullptr; glfwGetError(&err);

		Log::Error("Failed to init glfw: {}", err);
		throw std::runtime_error(fmt::format("Failed to init glfw: {}", err));
	}
	_initialized = true;

	Log::Info("glfw initialized");

	glfwSetErrorCallback(GLFWErrorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Log::Info("Creating glfw window '{}'...", GetTitle());
	window = glfwCreateWindow(GetWidth(), GetHeight(), GetTitle().c_str(), nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error(fmt::format("Failed to create glfw window!", GetTitle()));
	}
	Log::Info("glfw window '{}' created", GetTitle());

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	glfwSetWindowUserPointer(window, this);

	Log::Info("Loading OpenGL...");
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Log::Critical("Failed to initialize OpenGL context with GLAD");
		throw std::runtime_error("Failed to initialize OpenGL context");
	}

	Log::Info("OpenGL {} loaded", (const char*)glGetString(GL_VERSION));

	glfwSetFramebufferSizeCallback(window, GLFWFramebufferSizeCallback);

	SetWindowSize(width, height);

	Log::Info("Window '{}' created", GetTitle());
}

Window::~Window()
{
	Log::Info("Shutting down window '{}'", GetTitle());

	if (window)
	{
		Log::Info("Destroying glfw window '{}'...", GetTitle());
		glfwDestroyWindow(window);
		Log::Info("glfw window '{}' destroyed", GetTitle());
	}

	if (_initialized)
	{
		Log::Info("Terminating glfw...");
		glfwTerminate();
		Log::Info("glfw terminated");
	}

	Log::Info("Window '{}' shutdown", GetTitle());
}

void Window::PollEvents()
{
	if (_initialized)
	{
		glfwPollEvents();
	}
	else
	{
		Log::Warn("Polled events while glfw is uninitalized");
	}
}

void Window::Present()
{
	if (window)
	{
		glfwSwapBuffers(window);
	}
	else
	{
		Log::Warn("glfw window '{}' is null in Window::Present()", title);
	}
}

bool Window::ShouldClose() const
{
	if(window)
		return glfwWindowShouldClose(window);
	else
	{
		Log::Warn("glfw window '{}' is null in Window::ShouldClose(), treating as closed", title);
		return true;
	}
}

void Window::SetWindowSize(int _width, int _height)
{
	width = _width;
	height = _height;

	glViewport(0, 0, width, height);
}
