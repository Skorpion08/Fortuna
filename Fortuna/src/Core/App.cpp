#include "App.h"

#include "Platform/Window.h"
#include "Platform/GUI.h"

#include "Log.h"

#if defined(BACKEND_OPENGL)
#include "Renderer/OpenGL/RendererOpenGL.h"
#else
#error "No render backend selected"
#endif

App::App()
{
	Log::Info("Initalizing application...");

	window = std::make_unique<Window>(1200, 900, "Fortuna");

	Gui::Init(window.get());

	renderer = std::make_unique<RendererOpenGL>(1200.0f, 900.0f);

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

	sceneManager.TransitionTo(SceneType::MainMenu);

	float lastTime = window->GetTime();

	while (!window->ShouldClose())
	{
		float currentTime = window->GetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		window->PollEvents();

		renderer->StartFrame();
		Gui::StartFrame();

		renderer->Resize(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

		// logic
		sceneManager.Update(deltaTime);
		
		sceneManager.Render(renderer.get());
		renderer->Render();
		Gui::Render();

		window->Present();
	}

	running = false;
	Log::Info("Application stopped");
}