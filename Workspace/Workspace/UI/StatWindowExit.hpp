#pragma once
#include <VegaEngine2.h>
#include "FSM.h"

namespace fz {

	class StatWindowExitScript : public VegaScript
	{
	public:
		sf::Vector2f size;

		void Start() override
		{
			size = GetComponent<BoxCollider2DComponent>().GetSize();
		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			if (Input::IsMouseButtonPressed(MouseButtonType::Left))
			{
				if (IsClickedBounds())
				{
					if (HasComponent<ParentEntityComponent>())
					{
						GameObject parent = GetComponent<ParentEntityComponent>().ParentEntity;
						parent.SetActive(false);
					}
				}
			}
		}

		bool IsClickedBounds()
		{
			const sf::Vector2f& mousePos = GetCurrentScene()->GetWorldMousePos();
			const auto& pos = GetWorldPosition();
			sf::Vector2f bounds[2];
			bounds[0] = { pos.x - size.x * 2.f, pos.y - size.y * 2.f };
			bounds[1] = { pos.x + size.x * 2.f, pos.y + size.y * 2.f };
			if (bounds[0].x > mousePos.x || bounds[0].y > mousePos.y)
				return false;
			if (bounds[1].x < mousePos.x || bounds[1].y < mousePos.y)
				return false;
			return true;
		}
	};
} // namespace fz

