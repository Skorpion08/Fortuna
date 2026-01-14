#pragma once

class Window;

namespace Gui
{
bool IsInitialized();

void Init(Window* window);

void Shutdown();

void StartFrame();

void Render();
}