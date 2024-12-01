#pragma once
#include <VegaEngine2.h>

namespace fz {

	struct Stat
	{
		int HP = 100;
		int MP = 100;
		int AD = 0;
		int AP = 0;
		bool IsDead = false;
	};

	struct StatComponent
	{
	public:
		Stat Stat;

		StatComponent() = default;
		StatComponent(const StatComponent&) = default;
	};


}