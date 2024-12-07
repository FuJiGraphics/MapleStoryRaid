#pragma once
#include <VegaEngine2.h>
#include "BoundComponent.hpp"

namespace fz {

	class SkillWindowScript : public VegaScript
	{
	public:
		sf::Vector2f halfSize;
		sf::Vector2f prevMousePos;
		bool clicked = false;

		void Start() override
		{
			halfSize = GetComponent<BoxCollider2DComponent>().GetHalfSize();
		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			if (IsClickedBounds())
			{
				if (Input::IsMouseButtonPressed(MouseButtonType::Left))
				{
					if (!clicked)
					{
						clicked = true;
						prevMousePos = GetCurrentScene()->GetWorldMousePos();
					}
					else
					{
						const sf::Vector2f& mousePos = GetCurrentScene()->GetWorldMousePos();
						if (prevMousePos != mousePos)
						{
							auto& transformComp = GetComponent<TransformComponent>();
							sf::Vector2f nextPos = mousePos - prevMousePos;
							const auto& worldInverse = GetWorldTransform().getInverse().getMatrix();
							sf::Vector2f scale = { worldInverse[0], worldInverse[5] };
							nextPos = { nextPos.x * scale.x, nextPos.y * scale.y };

							const auto& currPos = transformComp.Transform.GetTranslate();
							transformComp.Transform.SetTranslate(currPos + nextPos);
							prevMousePos = mousePos;
						}
					}
				}
				else
				{
					clicked = false;
				}
			}
			else
				clicked = false;
		}

		bool IsClickedBounds()
		{
			auto& childComp = GetComponent<ChildEntityComponent>();
			for (auto& child : childComp.CurrentChildEntities)
			{
				if (child.HasComponent<BoundComponent>())
				{
					const auto& boundComp = child.GetComponent<BoundComponent>();
					// 바운드 클릭 시 드래그 할 수 없게 함
					if (boundComp.IsClicked)
						return false;
				}
			}

			const sf::Vector2f& mousePos = GetCurrentScene()->GetWorldMousePos();
			const auto& pos = GetWorldPosition();
			const auto& scale = GetWorldTransform().getMatrix();
			sf::Vector2f bounds[2];
			bounds[0] = { pos.x - (halfSize.x * scale[0]), pos.y - (halfSize.y * scale[5]) };
			bounds[1] = { pos.x + (halfSize.x * scale[0]), pos.y + (halfSize.y * scale[5]) };
			if (bounds[0].x > mousePos.x || bounds[0].y > mousePos.y)
				return false;
			if (bounds[1].x < mousePos.x || bounds[1].y < mousePos.y)
				return false;
			return true;
		}

	}; // class
} // namespace fz

