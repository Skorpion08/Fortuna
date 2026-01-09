#pragma once

#include <fstream>

enum class SceneType
{
	MainMenu,
	Game,
	Editor
};

class Scene
{
public:
	void Update();
private:
	void UpdateMainMenu();

	void UpdateGame();

	void UpdateEditor();

	void OpenQuestionFile();

	SceneType currentScene = SceneType::MainMenu;


};