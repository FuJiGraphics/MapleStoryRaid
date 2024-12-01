#pragma once
#include <VegaEngine2.h>
#include "Stat.hpp"

namespace fz {

	struct SkillComponent
	{
		Stat Stat;

		SkillComponent() = default;
		SkillComponent(const SkillComponent&) = default;
	};

}