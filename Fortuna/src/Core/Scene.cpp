#include "Scene.h"

#include "Platform/GUI.h"

void Scene::Update()
{
	if (currentScene == SceneType::MainMenu)
	{
		UpdateMainMenu();
		
	}
	else if (currentScene == SceneType::Game)
	{
		UpdateGame();
	}
	else if (currentScene == SceneType::Editor)
	{
		UpdateEditor();
	}
}

void Scene::UpdateMainMenu()
{
	Gui::MenuAction action = Gui::ShowMainMenu();
	if (action == Gui::MenuAction::Play)
	{
		currentScene = SceneType::Game;
	}
	else if (action == Gui::MenuAction::Editor)
	{
		currentScene = SceneType::Editor;
	}
}

void Scene::UpdateGame()
{

}

void Scene::UpdateEditor()
{
}
