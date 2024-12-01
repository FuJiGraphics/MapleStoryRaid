#pragma once
#include "Stat.hpp"

namespace fz {

	struct PortalComponent
	{
		sf::Vector2f NextPlayerPos;
		std::string NextScenePath;

		PortalComponent() = default;
		PortalComponent(const PortalComponent&) = default;
	};

} // namespace fz