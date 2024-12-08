#pragma once

#include <VegaEngine2.h>

namespace fz {

	struct SkillButtonComponent
	{
		std::string IconPath;
		GameObject Skill;

		SkillButtonComponent() = default;
		SkillButtonComponent(const SkillButtonComponent&) = default;
	};

}