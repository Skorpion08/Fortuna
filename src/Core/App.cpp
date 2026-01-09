#include "App.h"

#include "Platform/Window.h"
#include "Platform/GUI.h"

#include "Log.h"

App::App()
{
	Log::Info("Initalizing application...");

	window = std::make_unique<Window>(1200, 900, "Fortuna");

	Gui::Init(window.get());

	renderer = std::make_unique<Renderer<ChosenBackend>>();

	Log::Info("Application initialized");
}

App::~App()
{
	Log::Info("Shutting down application...");

	Gui::Shutdown();

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

		renderer->StartFrame();
		Gui::StartFrame();

		// logic
		
		renderer->Render();
		Gui::Render();

		window->Present();
	}

	running = false;
	Log::Info("Application stopped");
}