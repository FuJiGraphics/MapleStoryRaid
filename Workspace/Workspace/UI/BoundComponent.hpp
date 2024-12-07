#pragma once
#include <VegaEngine2.h>

namespace fz {

	struct BoundComponent
	{
		bool IsClicked = false;

		BoundComponent() = default;
		BoundComponent(const BoundComponent&) = default;
	};

}