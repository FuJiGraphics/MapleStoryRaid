#pragma once

#include <VegaEngine2.h>
#include "MapComponent.h"

namespace fz {

	class AmherstHuntScript : public VegaScript
	{
	public:
		void Start() override
		{
			auto& mapComp = AddComponent<MapComponent>();
			const std::string& tag = GetComponent<TagComponent>().Tag;
			const sf::Vector2f& pos = GetComponent<TransformComponent>().Transform.GetTranslate();
			const fz::Sprite& sprite = GetComponent<SpriteComponent>().Sprite;
			const sf::Vector2u& size = sprite.GetSize();
			mapComp.Tag = tag;
			mapComp.Position = pos;
			mapComp.Size.x = static_cast<float>(size.x);
			mapComp.Size.y = static_cast<float>(size.y);
			mapComp.Origin = sprite.GetOrigins();
		}
	}; // class 
} // namespace fz