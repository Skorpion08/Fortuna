#pragma once

#include <memory>

#include "Scenes/Scene.h"

class Renderer;

class SceneManager
{
public:
	void Update(float deltaTime);
	void Render(Renderer* renderer);
	void TransitionTo(SceneType next);
private:


	SceneType currentScene = SceneType::None;

	std::unique_ptr<Scene> sceneData;
};