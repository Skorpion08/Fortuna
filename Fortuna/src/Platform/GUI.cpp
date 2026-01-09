#include "GUI.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Core/Log.h"

#include "Window.h"

static bool _initialized = false;

namespace Gui
{
bool IsInitialized()
{
	return _initialized;
}

// Call this once at app start
void Init(Window* window)
{
	if (_initialized)
	{
		Log::Warn("Tried to initialize GUI again");
		return;
	}

	Log::Info("Initializing GUI...");

	Log::Info("Initializing DearImGui...");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends
	Log::Info("Initializing DearImGui backends...");

	ImGui_ImplGlfw_InitForOpenGL(window->GetImpl(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();
	
	Log::Info("DearImGui GLFW + OpenGL3 backend initialized");

	_initialized = true;
	Log::Info("GUI initalized");
}

void Shutdown()
{
	if (!_initialized)
	{
		Log::Warn("Tried to shutdown uninitialized GUI");
		return;
	}

	Log::Info("Shutting down GUI...");

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	_initialized = false;

	Log::Info("GUI shutdown");
}

void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

MenuAction ShowMainMenu()
{
	MenuAction result = MenuAction::None;

	if (ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground))
	{
		if (ImGui::Button("Gra", { 100, 30 }))
		{
			result = MenuAction::Play;
		}
		if(ImGui::Button("Edytor", { 100, 30 }))
		{
			result = MenuAction::Editor;
		}

		ImGui::End();
	}

	return result;
}

}