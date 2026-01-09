#pragma once

class RendererOpenGL
{
public:
	void Init();

	void Shutdown();

	void SetClearColor(float r, float g, float b, float a);

	void StartFrame();

	void Render();
};