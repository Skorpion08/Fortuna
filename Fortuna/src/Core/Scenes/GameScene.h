#pragma once

#include "Scene.h"

#include <array>
#include <unordered_set>

#include "Game/Question.h"
#include "Game/Phase.h"
#include "Game/Wheel.h"
#include "Game/Player.h"

inline constexpr std::array<char, 26> guessableLetters = {
	'a','b','c','d','e','f','g','h','i','j','k','l',
	'm','n','o','p','q','r','s','t','u','v','w','x','y','z'
};

inline constexpr std::array<char32_t, 26> guessableLetters32 = {
	U'a','b','c','d','e','f','g','h','i','j','k','l',
	'm','n','o','p','q','r','s','t','u','v','w','x','y','z'
};

inline constexpr std::array<char32_t, 26> guessableLettersUpper32 = {
	U'A','B','C','D','E','F','G','H','I','J','K','L',
	'M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
};

inline constexpr std::array<char32_t, 18> preguessedLetters = {
	U'Ą', U'ą', U'Ć', U'ć', U'Ę', U'ę', U'Ł', U'ł',
	U'Ń', U'ń', U'Ó', U'ó', U'Ś', U'ś', U'Ź', U'ź',
	U'Ż', U'ż'
};


inline constexpr bool ContainsLetter(char c)
{
	for (char l : guessableLetters)
		if (l == c) return true;
	return false;
}


inline constexpr bool ContainsLetter(char32_t c)
{
	for (char32_t l : guessableLetters32)
		if (l == c) return true;
	for (char32_t l : guessableLettersUpper32)
		if (l == c) return true;
	return false;
}

// Check if a codepoint is a pre-guessed Polish letter
inline constexpr bool IsPreguessed(char32_t c)
{
	for (char32_t l : preguessedLetters)
		if (l == c) return true;
	return false;
}

class Renderer;

class GameScene : public Scene
{
public:
	GameScene();

	virtual void Render(Renderer* renderer) override;
	virtual void Update() override;

	const Question& GetQuestion() const { return questions[questionIndex]; }

private:
	void StartRound(int nextQuestion);

	void UpdateStart();
	void UpdateWheel();
	void UpdateShowWheel();
	void UpdateGuessing();
	void UpdateShowGuessed();
	void UpdateNextRound();
	void UpdateResults();

	void ShowLeaderboard();
	void ShowEndLeaderboard();
	void ShowSentence();

	bool open = true;

	std::vector<Question> questions;

	int questionIndex = 0;

	std::vector<Player> players;
	char newNickname[128] = "";

	std::vector<int> endPoints;

	int currentPlayer = 0;
	Phase currentPhase = Phase::Start;

	int unguessedChars = 0;

	Slot* slotThisTurn = nullptr;
	int pointsToGain = 0;
	Wheel wheel;

	std::array<bool, guessableLetters.size()> usedLetters;

	std::string sentence;

	char guessedText[256] = "";
};