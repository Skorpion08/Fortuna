#pragma once

#include <vector>
#include <memory>
#include <random>

#include "Player.h"
#include "SpinResult.h"
#include "Slot.h"

class Wheel
{
public:
	Wheel();

	void AddSlot(std::unique_ptr<Slot>&& newSlot);

	Slot* Spin();
private:
	std::vector<std::unique_ptr<Slot>> slots;

	std::mt19937 engine{ std::random_device{}() };
};