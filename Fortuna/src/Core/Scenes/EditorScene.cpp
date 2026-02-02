#include "EditorScene.h"

#include "imgui.h"
#include "portable-file-dialogs.h"

#include "Core/Log.h"	

EditorScene::EditorScene()
{
	Log::Info("Transitioning to editor scene...");
	InitType(SceneType::Editor);

	Log::Info("Transitioned to editor scene");
}

EditorScene::~EditorScene()
{
}

void EditorScene::Update(float deltaTime)
{
	if (ImGui::Begin("Edytor", &open, ImGuiWindowFlags_MenuBar | (unsaved ? ImGuiWindowFlags_UnsavedDocument : 0)))
	{
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				if (ImGui::MenuItem("Nowy", "Ctrl+N"))
				{
					if (!unsaved)
					{
						NewFileDialog();
					}
					else
					{
						pendingAction = PendingAction::NewFile;
						openUnsavedPopup = true;
					}
				}

				if (ImGui::MenuItem("Otworz", "Ctrl+O"))
				{
					if (!unsaved)
					{
						OpenFileDialog();
					}
					else
					{
						pendingAction = PendingAction::OpenFile;
						openUnsavedPopup = true;
					}
				}

				if (ImGui::MenuItem("Zapisz", "Ctrl+S"))
				{
					SaveFile();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();

			bool ctrlS = io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false);
			if (ctrlS)
			{
				SaveFile();
			}

			bool ctrlO = io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_O, false);
			if (ctrlO)
			{
				if (!unsaved)
				{
					OpenFileDialog();
				}
				else
				{
					pendingAction = PendingAction::OpenFile;
					ImGui::OpenPopup("Unsaved Changes");
				}
			}
			bool ctrlN = io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_N, false);
			if (ctrlN)
			{
				if (!unsaved)
				{
					NewFileDialog();
				}
				else
				{
					pendingAction = PendingAction::NewFile;
					ImGui::OpenPopup("Unsaved Changes");
				}
			}

			if (openUnsavedPopup)
			{
				ImGui::OpenPopup("Unsaved Changes");
			}

			if (file.is_open())
			{
				if (ImGui::BeginTable("Questions", 3, ImGuiTableFlags_Resizable))
				{
					ImGui::TableSetupColumn("Kategoria");
					ImGui::TableSetupColumn("Tekst");
					ImGui::TableSetupColumn("Akcje");
					ImGui::TableHeadersRow();

					size_t row = 0;
					for (auto it = questions.begin(); it != questions.end(); )
					{
						Question& q = *it;

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);

						char bufCat[128];
						strncpy(bufCat, q.category.c_str(), sizeof(bufCat));
						if (ImGui::InputText(("##cat" + std::to_string(row)).c_str(), bufCat, sizeof(bufCat)))
						{
							q.category = bufCat; // update your vector
							unsaved = true;
						}

						ImGui::TableSetColumnIndex(1);

						char bufText[256];
						strncpy(bufText, q.text.c_str(), sizeof(bufText));
						if (ImGui::InputText(("##text" + std::to_string(row)).c_str(), bufText, sizeof(bufText)))
						{
							q.text = bufText; // update your vector
							unsaved = true;
						}

						ImGui::TableSetColumnIndex(2);
						if (ImGui::Button(("Usun##" + std::to_string(row)).c_str()))
						{
							it = questions.erase(it);
							unsaved = true;
							continue;
						}

						++it;
						++row;
					}

					ImGui::EndTable();
				}
				ImGui::Separator();

				ImGui::InputText("Nowa Kategoria", newCategory, sizeof(newCategory));
				ImGui::InputText("Nowy Tekst", newText, sizeof(newText));

				if (ImGui::Button("Dodaj Pytanie"))
				{
					if (strlen(newCategory) > 0 && strlen(newText) > 0)
					{
						questions.push_back({ newCategory, newText });
						newCategory[0] = '\0';
						newText[0] = '\0';

						unsaved = true;
					}
				}
			}
		}
	}
	ImGui::End();


	if (!open)
	{
		if (!unsaved) 
		{
			SetNextScene(SceneType::MainMenu);
		}
		else
		{
			pendingAction = PendingAction::Exit;
			ImGui::OpenPopup("Unsaved Changes");
		}
	}


	ShowUnsavedChangesPopup();
}

void EditorScene::ShowUnsavedChangesPopup()
{
	if (ImGui::BeginPopupModal("Unsaved Changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Masz niezapisane zmiany.");
		ImGui::Text("Czy chcesz je zapisac?");

		ImGui::Separator();

		if (ImGui::Button("Zapisz"))
		{
			SaveFile();
			unsaved = false;

			ExecutePendingAction();
			openUnsavedPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Nie zapisuj"))
		{
			unsaved = false;

			ExecutePendingAction();
			openUnsavedPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Anuluj"))
		{
			pendingAction = PendingAction::None;
			openUnsavedPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void EditorScene::ExecutePendingAction()
{
	switch (pendingAction)
	{
	case PendingAction::OpenFile:
		OpenFileDialog();
		break;

	case PendingAction::NewFile:
		NewFileDialog();
		break;

	case PendingAction::Exit:
		SetNextScene(SceneType::MainMenu);
		break;

	default:
		break;
	}

	pendingAction = PendingAction::None;
}

void EditorScene::NewFileDialog()
{
	auto result = pfd::save_file(
		"Create new questions file", // dialog title
		"new_file.json",           // default filename
		{ "JSON Files (.json)", "*.json" }
	).result();

	if (result.empty()) return;

	path = result;
	std::ofstream createFile(path);
	if (!createFile.is_open())
	{
		Log::Warn("Could not create a new file: {}", path);
	}

	file.open(path);
	if (!file.is_open())
	{
		Log::Warn("Newly created file could not be opened: {}", path);
	}

	nlohmann::json questionsData;

	questionsData = nlohmann::json::array();
	file << questionsData.dump(4);
	
	bool unsaved = false;
}

void EditorScene::OpenFileDialog()
{
	auto result = pfd::open_file(
		"Open questions file",
		".",                         // start dir
		{ "JSON Files (.json)", "*.json" }
	).result();

	if (result.empty())
		return; // cancelled

	LoadFile(result[0]);
}

void EditorScene::LoadFile(const std::string& filepath)
{
	Log::Info("Trying to open the questions file: {}", filepath);

	path = filepath;
	file.open(path);
	if (!file.is_open())
	{
		Log::Warn("Failed to open the questions file: {}", path);
		return;
	}

	nlohmann::json questionsData;

	try
	{
		file >> questionsData;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		Log::Warn("Failed to parse file: '{}', {}, initializing empty questions", path, e.what());

		file.close();
		return;
	}

	if (!questionsData.is_array())
	{
		Log::Warn("file: '{}', data is not an array (incorrect format)");

		questionsData = nlohmann::json::array();
	}

	int corrupted = 0;
	// Filter: only add valid entries
	for (size_t i = 0; i < questionsData.size(); ++i)
	{
		const auto& item = questionsData[i];
		try
		{
			Question q = item.get<Question>();
			questions.push_back(q);
		}
		catch (const nlohmann::json::exception& e)
		{
			++corrupted;

			Log::Warn("Skipped invalid question at index {}: {}", i, e.what());
			// skip this entry, do not add to vector
		}
	}

	Log::Info("Loaded {} questions", questions.size());
	if (corrupted > 0)
	{
		Log::Warn("Ignored {} corrupted elements", corrupted);
	}
}


void EditorScene::SaveFile()
{
	file.close();
	file.open(path, std::ios::out | std::ios::in | std::ios::trunc);

	if (!file.is_open())
	{
		Log::Info("Tried to update file, but it's not opened");
		return;
	}
	Log::Info("Updating questions file...");
	
	nlohmann::json questionsData = nlohmann::json::array();
	for (const auto& question : questions)
		questionsData.push_back(question);

	file << questionsData.dump(4);
	file.flush();
	unsaved = false;

	Log::Info("Questions file updated");
}
