#pragma once

enum class Phase
{
	Start = 0,

	Wheel,
	SpinningWheel,
	ShowWheel,
	Guessing,
	ShowGuessed,
	NextRound,

	Results
};