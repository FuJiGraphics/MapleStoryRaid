#pragma once
#include <VegaEngine2.h>
#include "FrameComponent.h"

namespace fz {

	class GameStartButtonScript : public VegaScript
	{
	public:
		sf::Vector2f halfSize;
		bool isClickedLeft = false;

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
					isClickedLeft = true;
					GetComponent<SpriteComponent>().Sprite.SetTexture(
						"game/graphics/title/CharSelect/CharSelectButton/Select/SelectPress.png");
				}
				else if (Input::IsMouseButtonReleased(MouseButtonType::Left))
				{
					if (isClickedLeft)
					{
						isClickedLeft = false;
						GetComponent<SpriteComponent>().Sprite.SetTexture(
							"game/graphics/title/CharSelect/CharSelectButton/Select/SelectRelease.png");
						GameObject title = GetCurrentScene()->GetEntityFromTag("TitleFrame");
						title.GetComponent<FrameComponent>().status = FrameStatus::CharSelectDone;
					}
				}
			}
			else
			{
				GetComponent<SpriteComponent>().Sprite.SetTexture(
					"game/graphics/title/CharSelect/CharSelectButton/Select/SelectHover.png");
			}
		}

		bool IsClickedBounds()
		{
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

	};
} // namespace fz

