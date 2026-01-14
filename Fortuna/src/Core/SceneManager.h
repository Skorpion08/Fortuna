#pragma once

#include <memory>

#include "Scenes/Scene.h"

class SceneManager
{
public:
	void Update();
	void TransitionTo(SceneType next);
private:


	SceneType currentScene = SceneType::None;

	std::unique_ptr<Scene> sceneData;
};