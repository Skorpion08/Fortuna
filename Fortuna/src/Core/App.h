#pragma once

#include <memory>

#include "SceneManager.h"

class Renderer;
class Window;

class App
{
public:
	App();

	~App();

	void Run();

protected:

	std::unique_ptr<Window> window;
	std::unique_ptr<Renderer> renderer;

	SceneManager sceneManager;

	bool running = false;
};