#pragma once

#include <string>

#include "Player.h"
#include "SpinResult.h"

class Slot
{
public:
	virtual ~Slot() = default;

	virtual SpinResult Use(Player& player) = 0;

	virtual const std::string& GetName() const = 0;

	// Whether the player can guess after getting this
	virtual bool CanGuess() const = 0;
};

class PointSlot : public Slot
{
public:
	PointSlot(int _points) : points(_points), title(std::to_string(_points))
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
	BankruptSlot() : title("Bankrupt")
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
	StopSlot() : title("Stop")
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