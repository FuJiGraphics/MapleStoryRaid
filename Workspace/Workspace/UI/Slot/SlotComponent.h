#pragma once
#include <VegaEngine2.h>

namespace fz {

	struct SlotComponent
	{
		std::string Tag;
		bool IsMounted = false;
		std::string SkillTag;
		KeyType Key;
		std::string SkillTexturePath;

		bool IsKeyDown() const
		{
			return Input::IsKeyDown(Key);
		}

		SlotComponent() = default;
		SlotComponent(const SlotComponent&) = default;
	};

}