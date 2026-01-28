#include "GameScene.h"

#include <nlohmann/json.hpp>
#include <imgui.h>

#include <string>
#include <vector>
#include <codecvt>
#include <locale>

std::u32string toUTF32(const std::string& utf8)
{
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	return conv.from_bytes(utf8);
}

std::string toUTF8(const std::u32string& utf32)
{
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	return conv.to_bytes(utf32);
}

#include "Core/Log.h"

GameScene::GameScene()
{
	Log::Info("Transitioning to game scene...");
	InitType(SceneType::Game);

	Log::Info("Loading questions...");
	std::ifstream questionsFile("questions.json");
	if (!questionsFile.is_open())
	{
		Log::Error("Failed to open questions.json");
		throw std::runtime_error("Failed to open questions.json");
	}
	Log::Info("Opened questions.json");

	nlohmann::json questionsData;
	try
	{
		questionsFile >> questionsData;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		Log::Error("Failed to parse questions.json: {}", e.what());
		throw std::runtime_error(fmt::format("Failed to parse questions.json: {}", e.what()));
	}

	if (!questionsData.is_array())
	{
		Log::Error("questions.json is not an array, initializing empty questions");
		throw std::runtime_error("questions.json is not an array, initializing empty questions");
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

	StartRound(0);

	Log::Info("Transitioned to game scene");
}

void GameScene::Render(Renderer* renderer)
{
	wheel.Render(renderer);
}

void GameScene::Update()
{
	if (!open)
	{
		Log::Info("Quit game, go to menu");
		currentPhase = Phase::Start;
		SetNextScene(SceneType::MainMenu);
		return;
	}

	switch (currentPhase)
	{
	case Phase::Start:
		UpdateStart();
		break;
	case Phase::Wheel:
		UpdateWheel();
		break;
	case Phase::ShowWheel:
		UpdateShowWheel();
		break;
	case Phase::Guessing:
		UpdateGuessing();
		break;
	case Phase::ShowGuessed:
		UpdateShowGuessed();
		break;
	case Phase::NextRound:
		UpdateNextRound();
		break;

	case Phase::Results:
		UpdateResults();
		break;

	default:
		Log::Error("No game phase defined, going back to the menu");
		currentPhase = Phase::Start;
		SetNextScene(SceneType::MainMenu);
	}
}

void GameScene::StartRound(int nextQuestion)
{
	assert(nextQuestion >= 0 && nextQuestion < questions.size());

	questionIndex = nextQuestion;

	usedLetters.fill(false);

	std::u32string question32 = toUTF32(GetQuestion().text);
	std::u32string sentence32(question32.size(), U'\0');

	for (size_t i = 0; i < question32.size(); ++i)
	{
		char32_t c = question32[i];

		if (c == U' ')
		{
			sentence32[i] = U' ';
		}
		else if (IsPreguessed(c))        // make sure IsPreguessed works with char32_t
		{
			sentence32[i] = c;
		}
		else if (ContainsLetter(c))      // make sure ContainsLetter works with char32_t
		{
			sentence32[i] = U'#';
			++unguessedChars;
		}
		else
		{
			Log::Warn("Unrecognized character in question: {}, pos: {}, character: {}", GetQuestion().text, i, static_cast<char>(c));
		}
	}

	// Convert back to UTF-8 to store in your std::string
	sentence = toUTF8(sentence32);
}

void GameScene::UpdateStart()
{
	if (ImGui::Begin("Start", &open))
	{
		if (ImGui::BeginTable("Players", 1, ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Nickname");
			ImGui::TableHeadersRow();

			size_t row = 0;
			for (auto it = players.begin(); it != players.end(); )
			{
				Player& player = *it;

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);

				char bufNick[128];
				strncpy(bufNick, player.nickname.c_str(), sizeof(bufNick));
				if (ImGui::InputText(("##nick" + std::to_string(row)).c_str(), bufNick, sizeof(bufNick)))
				{
					player.nickname = bufNick; // update your vector
				}

				ImGui::SameLine();
				if (ImGui::Button(("Usun##" + std::to_string(row)).c_str()))
				{
					it = players.erase(it);
					continue;
				}

				++it;
				++row;
			}

			ImGui::EndTable();
		}
		ImGui::Separator();

		bool addNew = ImGui::InputText("Nowy nick", newNickname, sizeof(newNickname), ImGuiInputTextFlags_EnterReturnsTrue);

		if (ImGui::Button("Dodaj gracza") || addNew)
		{
			if (strlen(newNickname) > 0)
			{
				players.push_back({ newNickname });
				newNickname[0] = '\0';
			}
		}

		ImGui::Separator();

		if (ImGui::Button("Start"))
		{
			endPoints.resize(players.size(), 0);

			currentPhase = Phase::Wheel;
		}
	}
	ImGui::End();
}

void GameScene::UpdateWheel()
{
	ShowLeaderboard();
	ShowSentence();

	if (ImGui::Begin("Controls", &open))
	{
		ImGui::Text("Current player: %s", players[currentPlayer].nickname.c_str());

		if (ImGui::Button("Spin"))
		{
			slotThisTurn = wheel.Spin();
			currentPhase = Phase::ShowWheel;
		}
	}
	ImGui::End();
}

void GameScene::UpdateShowWheel()
{
	ShowLeaderboard();
	ShowSentence();

	if (ImGui::Begin("Controls", &open))
	{
		ImGui::Text("Current player: %s", players[currentPlayer].nickname.c_str());

		ImGui::Text("Wylosowany slot: %s", slotThisTurn->GetName().c_str());
		if (slotThisTurn->CanGuess())
		{
			if (ImGui::Button("Guess"))
			{
				SpinResult result = slotThisTurn->Use(players[currentPlayer]);
				pointsToGain = result.points;
				currentPhase = Phase::Guessing;

				Log::Info("{} proceeds to guess", players[currentPlayer].nickname);
			}
		}
		else
		{
			if (ImGui::Button("Skip"))
			{
				SpinResult result = slotThisTurn->Use(players[currentPlayer]);
				currentPhase = Phase::ShowGuessed;

				Log::Info("{} skips turn", players[currentPlayer].nickname);
			}
		}
	}
	ImGui::End();
}

void GameScene::UpdateGuessing()
{
	ShowLeaderboard();
	ShowSentence();

	if (ImGui::Begin("Controls", &open))
	{
		ImGui::Text("Current player: %s", players[currentPlayer].nickname.c_str());
		ImGui::Text("Points to gain: %d", pointsToGain);

		ImGui::Separator();

		ImGui::InputText("GuessedText", guessedText, sizeof(guessedText));

		if (ImGui::Button("Odgagnij haslo"))
		{
			std::string guessed = guessedText;
			if (guessed == GetQuestion().text)
			{
				Log::Info("Player '{}' guessed the password", players[currentPlayer].nickname);

				endPoints[currentPlayer] += players[currentPlayer].points;

				if (questionIndex + 1 < questions.size())
				{
					currentPhase = Phase::NextRound;
				}
				else
				{
					//End of the game
					currentPhase = Phase::Results;
				}

			}
			else
			{
				Log::Info("Player '{}' failed to guess the password", players[currentPlayer].nickname);
				currentPhase = Phase::ShowGuessed;
			}

			guessedText[0] = '\0';
		}

		ImGui::Separator();

		float buttonSize = 40.0f;
		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float width = ImGui::GetContentRegionAvail().x;
		int columns = (int)(width / (buttonSize + spacing));
		if (columns < 1) columns = 1;

		bool sameLine = false;

		for (int i = 0; i < guessableLetters.size(); ++i)
		{
			const char letter = guessableLetters[i];
			const std::string label =
				(usedLetters[i] ? "[ ]" : std::string(1, letter))
				+ "##"
				+ std::string(1, letter);

			if (ImGui::Button(label.c_str(), ImVec2(buttonSize, buttonSize)))
			{
				if (!usedLetters[i])
				{
					std::u32string question32 = toUTF32(GetQuestion().text);
					std::u32string sentence32 = toUTF32(sentence);

					const char32_t letter32 = guessableLetters32[i];
					const char32_t upper32 = guessableLettersUpper32[i];
					for (size_t j = 0; j < sentence32.size(); ++j)
					{
						const char32_t answer = question32[j];
						char32_t& current = sentence32[j];

						if (current != U'#') continue;

						// guessed correctly
						if (answer == letter32 || answer == upper32)
						{
							current = answer;
							players[currentPlayer].points += pointsToGain;
							--unguessedChars;
						}
					}

					sentence = toUTF8(sentence32);

					usedLetters[i] = true;
					currentPhase = Phase::ShowGuessed;
					guessedText[0] = '\0';
				}
			}

			sameLine = false;
			if ((i + 1) % columns != 0)
			{
				ImGui::SameLine();
				sameLine = true;
			}
		}
		// So we break the sameline shit
		if(sameLine) ImGui::NewLine();
		ImGui::Separator();
	}
	ImGui::End();
}

void GameScene::UpdateShowGuessed()
{
	ShowLeaderboard();
	ShowSentence();

	if (ImGui::Begin("Controls", &open))
	{
		int nextPlayer = (currentPlayer + 1) % players.size();

		ImGui::Text("Next player: %s", players[nextPlayer].nickname.c_str());

		if (ImGui::Button("NextPlayer"))
		{
			currentPlayer = nextPlayer;

			currentPhase = Phase::Wheel;
			Log::Info("Set to next player's turn: {}", players[currentPlayer].nickname);
		}
	}
	ImGui::End();
}

void GameScene::UpdateNextRound()
{
	ShowEndLeaderboard();

	if (ImGui::Begin("Controls", &open))
	{
		ImGui::Text("Starting player: %s", players[currentPlayer].nickname.c_str());

		if (ImGui::Button("NextRound"))
		{
			for (auto i = 0; i < players.size(); ++i)
			{
				players[i].points = 0;
			}

			StartRound(questionIndex + 1);

			currentPhase = Phase::Wheel;
			Log::Info("Set to next player's turn: {}", players[currentPlayer].nickname);
		}
	}
	ImGui::End();
}

void GameScene::UpdateResults()
{
	ShowEndLeaderboard();

	if (ImGui::Begin("Controls", &open))
	{
		if (ImGui::Button("Menu"))
		{
			currentPhase = Phase::Start;
			SetNextScene(SceneType::MainMenu);

			Log::Info("Game end, going to menu");
		}
	}
	ImGui::End();
}

void GameScene::ShowLeaderboard()
{
	if (ImGui::Begin("Leaderboard"))
	{
		// Create index list
		std::vector<size_t> order(players.size());
		std::iota(order.begin(), order.end(), 0);

		// Sort indices by points (descending)
		std::sort(order.begin(), order.end(),
			[this](size_t a, size_t b)
			{
				return players[a].points > players[b].points;
			});

		for (size_t rank = 0; rank < order.size(); ++rank)
		{
			const Player& p = players[order[rank]];
			ImGui::Text("%zu. %s - %d", rank + 1, p.nickname.c_str(), p.points);
		}
	}
	ImGui::End();
}

void GameScene::ShowEndLeaderboard()
{
	if (ImGui::Begin("Leaderboard"))
	{
		// Create index list
		std::vector<size_t> order(players.size());
		std::iota(order.begin(), order.end(), 0);

		// Sort indices by points (descending)
		std::sort(order.begin(), order.end(),
			[this](size_t a, size_t b)
			{
				return endPoints[a] > endPoints[b];
			});

		for (size_t rank = 0; rank < order.size(); ++rank)
		{
			const Player& p = players[order[rank]];
			ImGui::Text("%zu. %s - %d", rank + 1, p.nickname.c_str(), endPoints[order[rank]]);
		}
	}
	ImGui::End();
}

void GameScene::ShowSentence()
{
	if (ImGui::Begin("Sentence"))
	{
		ImGui::Text(GetQuestion().category.c_str());

		ImGui::Separator();

		ImGui::Text(sentence.c_str());
	}
	ImGui::End();
}
