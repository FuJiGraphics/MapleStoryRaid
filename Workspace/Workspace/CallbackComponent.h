#pragma once
#include <VegaEngine2.h>

namespace fz {

	struct CallbackComponent
	{
		std::unordered_map<std::string, std::vector<std::function<void(GameObject)>>> Callbacks;

		CallbackComponent() = default;
		CallbackComponent(const CallbackComponent&) = default;
	};

}