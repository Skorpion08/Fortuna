#include "EditorScene.h"

#include "imgui.h"

#include "Core/Log.h"	

EditorScene::EditorScene()
{
	Log::Info("Transitioning to editor scene...");
	InitType(SceneType::Editor);


	nlohmann::json questionsData;

	Log::Info("Trying to open the questions.json file...");
	std::ifstream questionsFile("questions.json");
	if (!questionsFile.is_open())
	{
		std::ofstream createFile("questions.json"); // creates empty file
		questionsData = nlohmann::json::array();
		createFile << questionsData.dump(4);
		createFile.close();

		Log::Info("Created the questions.json file");
		questionsFile.open("questions.json", std::ios::in);
		if (!questionsFile.is_open())
			throw std::runtime_error("Failed to open questions.json even after creating it");
	}


	try
	{
		questionsFile >> questionsData;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		Log::Warn("Failed to parse questions.json: {}, initializing empty questions", e.what());

		questionsData = nlohmann::json::array();
	}

	if (!questionsData.is_array())
	{
		Log::Warn("questions.json is not an array, initializing empty questions");

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

	Log::Info("Transitioned to editor scene");
}

EditorScene::~EditorScene()
{
}

void EditorScene::Update()
{
	bool open = true;
	if (ImGui::Begin("Edytor", &open, ImGuiWindowFlags_MenuBar | (unsaved ? ImGuiWindowFlags_UnsavedDocument : 0)))
	{
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{

				if (ImGui::MenuItem("Zapisz", "Ctrl+S"))
				{
					SaveFile();
					unsaved = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();

			bool ctrlS = io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false);
			if (ctrlS)
			{
				SaveFile();
				unsaved = false;
			}

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
	ImGui::End();

	if (!open)
	{
		SetNextScene(SceneType::MainMenu);
	}
}

void EditorScene::SaveFile()
{
	Log::Info("Updating questions file...");

	std::ofstream questionsFile("questions.json", std::ios::out | std::ios::trunc);
	if (!questionsFile.is_open())
	{
		Log::Error("Could not open questions.json file");
		throw std::runtime_error("Failed to open questions.json file");
	}

	nlohmann::json questionsData = nlohmann::json::array();
	for (const auto& question : questions)
		questionsData.push_back(question);

	questionsFile << questionsData.dump(4);

	Log::Info("Questions file updated");
}
