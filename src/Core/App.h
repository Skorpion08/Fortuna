#pragma once

#include <memory>

#include "Renderer/Backend.h"

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

	bool running = false;
};