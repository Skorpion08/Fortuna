#include "Wheel.h"

#include "Slot.h"

#include "Core/Log.h"

#include "Renderer/Renderer.h"
#include <numbers>

constexpr float PI = std::numbers::pi_v<float>;

Wheel::Wheel()
{
	Log::Info("Initializing wheel");

	glm::vec4 color100 = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);  // red
	glm::vec4 color150 = glm::vec4(0.1f, 0.8f, 0.1f, 1.0f);  // green
	glm::vec4 color200 = glm::vec4(0.1f, 0.1f, 0.8f, 1.0f);  // blue
	glm::vec4 color250 = glm::vec4(0.8f, 0.8f, 0.1f, 1.0f);  // yellow
	glm::vec4 color300 = glm::vec4(0.8f, 0.1f, 0.8f, 1.0f);  // magenta
	glm::vec4 color350 = glm::vec4(0.1f, 0.8f, 0.8f, 1.0f);  // cyan
	glm::vec4 color400 = glm::vec4(0.5f, 0.3f, 0.7f, 1.0f);  // purple
	glm::vec4 color500 = glm::vec4(0.9f, 0.5f, 0.2f, 1.0f);  // orange
	glm::vec4 color1500 = glm::vec4(0.2f, 0.5f, 0.9f, 1.0f);  // sky blue
	glm::vec4 color2500 = glm::vec4(0.4f, 0.9f, 0.2f, 1.0f);  // lime green

	AddSlot(std::make_unique<PointSlot>(400, color400));
	AddSlot(std::make_unique<PointSlot>(2500, color2500));
	AddSlot(std::make_unique<PointSlot>(250, color250));
	AddSlot(std::make_unique<StopSlot>());
	AddSlot(std::make_unique<PointSlot>(400, color400));
	AddSlot(std::make_unique<PointSlot>(200, color200));
	AddSlot(std::make_unique<PointSlot>(100, color100));

	AddSlot(std::make_unique<PointSlot>(500, color500));

	AddSlot(std::make_unique<PointSlot>(150, color150));
	AddSlot(std::make_unique<PointSlot>(250, color250));
	AddSlot(std::make_unique<PointSlot>(300, color300));

	AddSlot(std::make_unique<PointSlot>(100, color100));

	AddSlot(std::make_unique<PointSlot>(200, color200));
	AddSlot(std::make_unique<BankruptSlot>());

	AddSlot(std::make_unique<PointSlot>(1500, color1500));
	AddSlot(std::make_unique<PointSlot>(350, color350));

	AddSlot(std::make_unique<PointSlot>(500, color500)); // tu powinno byc ?500 ale chuj nie chce mi sie

	AddSlot(std::make_unique<PointSlot>(200, color200));
	
	AddSlot(std::make_unique<PointSlot>(150, color150));
	AddSlot(std::make_unique<PointSlot>(200, color200));
	AddSlot(std::make_unique<PointSlot>(200, color200)); // tu nagroda

	AddSlot(std::make_unique<PointSlot>(250, color250));
	AddSlot(std::make_unique<PointSlot>(500, color500));

	AddSlot(std::make_unique<BankruptSlot>());

	colors.resize(slots.size());
	for (auto i = 0; i < colors.size(); ++i)
	{
		colors[i] = slots[i]->GetColor();
	}

	Log::Info("Wheel initialized");
}

void Wheel::Render(Renderer* renderer)
{
	renderer->RenderWheel(slots.size(), glm::vec2(0.0f, 0.0f), 350.0f, rotation, colors);
}

void Wheel::AddSlot(std::unique_ptr<Slot>&& newSlot)
{
	slots.push_back(std::move(newSlot));
	Log::Info("Added new slot nr.{} {}", slots.size() - 1, slots.back()->GetName());
}

void Wheel::StartSpin()
{
	Log::Info("Spinning wheel...");

	if (slots.size() <= 0)
	{
		Log::Error("Wheel has no slots");
		throw std::runtime_error("Wheel has no slots");
	}

	spinTime = 3.0f;
	velocity = 6.0f * PI;

	std::uniform_int_distribution<int> dist(0, slots.size() - 1);
	pickedIndex = dist(engine);

	float slotAngle = (2.0f * PI) / static_cast<float>(slots.size());

	desiredRotation = (slots.size() - 1 - pickedIndex) * slotAngle + slotAngle / 2.0f;
}

bool Wheel::Spinning(float deltaTime)
{
	// If still spinning with initial velocity
	if (spinTime > 0.0f)
	{
		spinTime -= deltaTime;

		rotation = std::fmod(rotation, 2.0f * PI);
		if (rotation < 0.0f) rotation += 2.0f * PI;

		// Set target to desired rotation plus a few extra spins
		chasedRotation = desiredRotation + 6.0f * PI;

		// Apply initial velocity
		rotation += velocity * deltaTime;

		return true;
	}

	// Remaining distance to target
	float distance = chasedRotation - rotation;

	if (distance > 0.001f)
	{
		// Apply a simple proportional deceleration
		float deceleration = 1.0f; // higher = slows down faster
		velocity = distance * deceleration; // always points toward target

		// Optional: cap max velocity so it doesn't jump
		float maxVel = 6.0f * PI;
		if (velocity > maxVel) velocity = maxVel;

		// Update rotation
		rotation += velocity * deltaTime;

		// Clamp overshoot
		if (rotation > chasedRotation)
		{
			rotation = chasedRotation;
			velocity = 0.0f;
		}

		return true;
	}

	// Target reached
	rotation = desiredRotation;
	velocity = 0.0f;
	return false;
}

Slot* Wheel::EndSpin()
{
	float slotAngle = (2.0f * PI) / static_cast<float>(slots.size());

	// Use FINAL rotation, not desiredRotation
	float angle = std::fmod(rotation, 2.0f * PI);
	if (angle < 0.0f) angle += 2.0f * PI;

	int index = static_cast<int>(angle / slotAngle);
	index = std::clamp(index, 0, static_cast<int>(slots.size() - 1));
	index = slots.size() - 1 - index;

	assert(index == pickedIndex);

	Log::Info("Determined slot: {}", slots[index]->GetName());
	return slots[index].get();
}

void Wheel::ResetSpin()
{
	rotation = 0.0f;
	velocity = 0.0f;
}
