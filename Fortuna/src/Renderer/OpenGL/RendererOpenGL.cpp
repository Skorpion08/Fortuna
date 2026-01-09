#include "RendererOpenGL.h"

#include "Core/Log.h"

#include "glad/glad.h"

void RendererOpenGL::Init()
{
	Log::Info("Initializing renderer's OpenGL backend...");

	SetClearColor(0.4f, 0.5f, 0.6f, 1.0f);

	Log::Info("Renderer's OpenGL backend initialized");
}

void RendererOpenGL::Shutdown()
{
	Log::Info("Shutting down OpenGL renderer");



	Log::Info("OpenGL renderer shutdown");
}

void RendererOpenGL::SetClearColor(float r, float g, float b, float a)
{
	Log::Info("Clear color set to: {}, {}, {}, {}", r, g, b, a);

	glClearColor(r, g, b, a);
}

void RendererOpenGL::StartFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererOpenGL::Render()
{

}