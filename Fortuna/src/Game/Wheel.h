#pragma once

#include <vector>
#include <memory>
#include <random>

#include "Player.h"
#include "SpinResult.h"
#include "Slot.h"

class Renderer;

class Wheel
{
public:
	Wheel();

	void Render(Renderer* renderer);

	void AddSlot(std::unique_ptr<Slot>&& newSlot);

	void StartSpin();
	bool Spinning(float deltaTime);
	Slot* EndSpin();

	void ResetSpin();
private:
	std::vector<std::unique_ptr<Slot>> slots;
	std::vector<glm::vec4> colors;

	std::mt19937 engine{ std::random_device{}() };

	int pickedIndex;
	float desiredRotation;
	float chasedRotation;

	float rotation=0.0f;
	float velocity = 0.0f;
	float spinTime = 0.0f;
};