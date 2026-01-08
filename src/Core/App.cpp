#include "App.h"

#include "../Platform/Window.h"

#include "Log.h"

App::App()
{
	Log::Info("Initalizing application...");

	window = std::make_unique<Window>(400, 600, "Fortuna");

	Log::Info("Application initialized");
}

App::~App()
{
	Log::Info("Shutting down application...");

	if (window)
		window.reset();

	Log::Info("Application shutdown");
}

void App::Run()
{
	running = true;
	Log::Info("Application running...");

	while (!window->ShouldClose())
	{
		window->PollEvents();

		window->Present();
	}

	running = false;
	Log::Info("Application stopped");
}