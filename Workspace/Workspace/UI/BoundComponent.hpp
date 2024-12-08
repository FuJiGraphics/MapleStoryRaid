#pragma once
#include <VegaEngine2.h>

namespace fz {

	struct BoundComponent
	{
		bool IsClicked = false;
		sf::FloatRect GlobalBounds;

		BoundComponent() = default;
		BoundComponent(const BoundComponent&) = default;
	};

}