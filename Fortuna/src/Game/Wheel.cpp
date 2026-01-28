#include "Wheel.h"

#include "Slot.h"

#include "Core/Log.h"

#include "Renderer/Renderer.h"

Wheel::Wheel()
{
	Log::Info("Initializing wheel");

	glm::vec4 color100 = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
	glm::vec4 color150 = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
	glm::vec4 color200 = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
	glm::vec4 color250 = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
	glm::vec4 color300 = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
	glm::vec4 color350 = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
	glm::vec4 color400 = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
	glm::vec4 color500 = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
	glm::vec4 color1500 = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
	glm::vec4 color2500 = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);

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
	renderer->RenderWheel(slots.size(), glm::vec2(0.0f, 0.0f), 300.0f, rotation, colors);
}

void Wheel::AddSlot(std::unique_ptr<Slot>&& newSlot)
{
	slots.push_back(std::move(newSlot));
	Log::Info("Added new slot nr.{} {}", slots.size() - 1, slots.back()->GetName());
}

Slot* Wheel::Spin()
{
	Log::Info("Spinning wheel...");
	if (slots.size() <= 0)
	{
		Log::Error("Wheel has no slots");
		throw std::runtime_error("Wheel has no slots");
	}

	std::uniform_int_distribution<int> dist(0, slots.size() - 1);

	int index = dist(engine);
	Log::Info("Determined slot: {}", slots[index]->GetName());
	return slots[index].get();
}
