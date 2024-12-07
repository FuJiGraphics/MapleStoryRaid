#pragma once
#include <VegaEngine2.h>

namespace fz {

	struct MapComponent
	{
	public:
		std::string Tag;
		sf::Vector2f Position;
		sf::Vector2f Size;
		Origins	Origin;

		MapComponent() = default;
		MapComponent(const MapComponent&) = default;
	};

} // namespace fz