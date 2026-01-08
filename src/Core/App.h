#pragma once

#include <memory>

class Window;

class App
{
public:
	App();

	virtual ~App();

	void Run();

protected:

	std::unique_ptr<Window> window;

	bool running = false;
};