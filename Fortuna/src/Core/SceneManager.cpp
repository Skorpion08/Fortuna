#include "SceneManager.h"

#include "Platform/GUI.h"

#include "Scenes/MainMenuScene.h"
#include "Scenes/GameScene.h"
#include "Scenes/EditorScene.h"

#include "Core/Log.h"

void SceneManager::Update()
{
	sceneData->Update();

	if (sceneData->WantsToTransition())
		TransitionTo(sceneData->GetNextScene());
}

void SceneManager::TransitionTo(SceneType next)
{
	if (next == currentScene)
	{
		Log::Warn("Scene tried to transition to itself");
		return;
	}
	Log::Info("Scene transitioning...");

	std::unique_ptr<Scene> newData;

	try
	{
		switch (next)
		{
		case SceneType::MainMenu: newData = std::make_unique<MainMenuScene>(); break;
		case SceneType::Game: newData = std::make_unique<GameScene>(); break;
		case SceneType::Editor: newData = std::make_unique<EditorScene>(); break;

		default: throw std::invalid_argument("Next scene was set to None");
		}
	}
	catch (std::exception& e) // We report the error and go back
	{
		Log::Error("Failed to transition scene: {}", e.what());
		sceneData->SetNextScene(sceneData->GetType());
		return;
	}

	// success
	currentScene = next;
	sceneData = std::move(newData);

	Log::Info("Scene transitioned");
}