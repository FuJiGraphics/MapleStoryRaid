#pragma once

#include <VegaEngine2.h>

namespace fz {

	struct SlotComponent
	{
		std::string Tag;
		bool IsMounted = false;
		GameObject Skill;

		SlotComponent() = default;
		SlotComponent(const SlotComponent&) = default;
	};

}