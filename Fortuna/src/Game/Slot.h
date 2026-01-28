#pragma once

#include <string>

#include "Player.h"
#include "SpinResult.h"

#include <glm/vec4.hpp>

class Slot
{
public:
	Slot(glm::vec4 _color) : color(_color) {}

	virtual ~Slot() = default;

	virtual SpinResult Use(Player& player) = 0;

	virtual const std::string& GetName() const = 0;

	// Whether the player can guess after getting this
	virtual bool CanGuess() const = 0;

	glm::vec4 GetColor() const { return color; }

private:
	glm::vec4 color;
};

class PointSlot : public Slot
{
public:
	PointSlot(int _points, glm::vec4 _color) : Slot(_color), points(_points), title(std::to_string(_points))
	{
	}

	virtual SpinResult Use(Player& player) override
	{
		SpinResult result;
		result.canGuess = CanGuess();
		result.points = points;
		return result;
	}

	virtual const std::string& GetName() const override { return title; }

	// Whether the player can guess after getting this
	virtual bool CanGuess() const override { return true; }

private:
	int points;
	std::string title;
};

class BankruptSlot : public Slot
{
public:
	BankruptSlot() : Slot(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f)), title("Bankrupt")
	{
	}

	virtual SpinResult Use(Player& player) override
	{
		SpinResult result;

		result.canGuess = CanGuess();
		result.points = 0;

		player.points = 0;

		return result;
	}

	virtual const std::string& GetName() const override { return title; }

	// Whether the player can guess after getting this
	virtual bool CanGuess() const override { return false; }

private:
	std::string title;
};

class StopSlot : public Slot
{
public:
	StopSlot() : Slot(glm::vec4(0.45f, 0.7f, 0.8f, 1.0f)), title("Stop")
	{
	}

	virtual SpinResult Use(Player& player) override
	{
		SpinResult result;

		result.canGuess = CanGuess();
		result.points = 0;

		return result;
	}

	virtual const std::string& GetName() const override { return title; }

	// Whether the player can guess after getting this
	virtual bool CanGuess() const override { return false; }

private:
	std::string title;
};