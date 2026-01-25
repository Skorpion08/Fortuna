#include "Wheel.h"

#include "Slot.h"

#include "Core/Log.h"

Wheel::Wheel()
{
	Log::Info("Initializing wheel");

	AddSlot(std::make_unique<PointSlot>(400));
	AddSlot(std::make_unique<PointSlot>(2500));
	AddSlot(std::make_unique<PointSlot>(250));
	AddSlot(std::make_unique<StopSlot>());
	AddSlot(std::make_unique<PointSlot>(400));
	AddSlot(std::make_unique<PointSlot>(200));
	AddSlot(std::make_unique<PointSlot>(100));

	AddSlot(std::make_unique<PointSlot>(500));

	AddSlot(std::make_unique<PointSlot>(150));
	AddSlot(std::make_unique<PointSlot>(250));
	AddSlot(std::make_unique<PointSlot>(300));

	AddSlot(std::make_unique<PointSlot>(100));

	AddSlot(std::make_unique<PointSlot>(200));
	AddSlot(std::make_unique<BankruptSlot>());

	AddSlot(std::make_unique<PointSlot>(1500));
	AddSlot(std::make_unique<PointSlot>(350));

	AddSlot(std::make_unique<PointSlot>(500)); // tu powinno byc ?500 ale chuj nie chce mi sie

	AddSlot(std::make_unique<PointSlot>(200));
	
	AddSlot(std::make_unique<PointSlot>(150));
	AddSlot(std::make_unique<PointSlot>(200));
	AddSlot(std::make_unique<PointSlot>(200)); // tu nagroda

	AddSlot(std::make_unique<PointSlot>(250));
	AddSlot(std::make_unique<PointSlot>(500));

	AddSlot(std::make_unique<BankruptSlot>());

	Log::Info("Wheel initialized");
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
