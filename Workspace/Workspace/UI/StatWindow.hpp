#pragma once
#include <VegaEngine2.h>
#include "FSM.h"

namespace fz {

	class StatWindowScript : public VegaScript
	{
	public:
		sf::Vector2f size;
		sf::Vector2f prevMousePos;
		bool clicked = false;

		void Start() override
		{
			size = GetComponent<BoxCollider2DComponent>().GetSize();
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

