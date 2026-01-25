#include "MainMenuScene.h"

#include "imgui.h"

#include "Core/Log.h"

MainMenuScene::MainMenuScene()
{
	Log::Info("Transitioning to main menu scene...");
	InitType(SceneType::MainMenu);

}

void MainMenuScene::Update()
{
	if (ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground))
	{
		if (ImGui::Button("Gra", { 100, 30 }))
		{
			SetNextScene(SceneType::Game);
		}
		if (ImGui::Button("Edytor", { 100, 30 }))
		{
			SetNextScene(SceneType::Editor);
		}

	}
	ImGui::End();

}
