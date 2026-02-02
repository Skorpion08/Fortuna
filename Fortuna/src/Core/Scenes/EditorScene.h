#pragma once

#include "Scene.h"

#include <fstream>
#include "nlohmann/json.hpp"

#include <vector>

#include "Game/Question.h"

enum class PendingAction
{
	None,
	OpenFile,
	NewFile,
	Exit
};

class EditorScene : public Scene
{
public:
	EditorScene();

	virtual ~EditorScene();

	virtual void Update(float deltaTime) override;

private:

	void ShowUnsavedChangesPopup();

	void ExecutePendingAction();

	void NewFileDialog();

	void OpenFileDialog();

	void LoadFile(const std::string& filepath);

	void SaveFile();

	std::fstream file;
	std::string path;

	std::vector<Question> questions;

	bool unsaved = false;

	char newCategory[128] = ""; 
	char newText[256] = "";

	bool open = true;

	PendingAction pendingAction = PendingAction::None;
	bool openUnsavedPopup = false;
};