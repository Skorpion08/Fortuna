#pragma once

#include "Scene.h"

#include <fstream>
#include "nlohmann/json.hpp"

#include <vector>

#include "Game/Question.h"

class EditorScene : public Scene
{
public:
	EditorScene();

	virtual ~EditorScene();

	virtual void Update() override;

private:

	void SaveFile();

	std::vector<Question> questions;

	bool unsaved = false;

	char newCategory[128] = ""; 
	char newText[256] = "";
};