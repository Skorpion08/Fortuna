#pragma once

#include <string>

struct GLFWwindow;

class Window
{
public:
	Window(int _width, int _height, const std::string& _title);

	virtual ~Window();

	void PollEvents();

	void Present();

	bool ShouldClose() const;

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	void SetWindowSize(int _width, int _height);

	const std::string& GetTitle() const { return title; }

	GLFWwindow* GetImpl() { return window; }
	const GLFWwindow* GetImpl() const { return window; }

	float GetTime() const;

private:

	int width, height;
	std::string title;

	GLFWwindow* window;

	bool _initialized = false;


};