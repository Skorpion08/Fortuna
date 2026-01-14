#pragma once

#include <memory>

#include "Renderer/Backend.h"

#include "SceneManager.h"

class Window;

class App
{
public:
	App();

	~App();

	void Run();

protected:

	std::unique_ptr<Window> window;
	std::unique_ptr<Renderer<ChosenBackend>> renderer;

	SceneManager sceneManager;

	bool running = false;
};