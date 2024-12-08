#pragma once
#include <VegaEngine2.h>

namespace fz {

	class SkillUIScript : public VegaScript
	{
	public:
		sf::Vector2f halfSize;
		bool isClickedLeft = false;
		GameObject skillWindow;

		void Start() override
		{
			halfSize = GetComponent<BoxCollider2DComponent>().GetHalfSize();
			skillWindow = GetCurrentScene()->GetEntityFromTag("SkillWindow");
		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			if (skillWindow.GetActive())
				GetComponent<SpriteComponent>().Sprite.SetColor({ 180, 180, 180, 255 });
			else
				GetComponent<SpriteComponent>().Sprite.SetColor({ 255, 255, 255, 255 });

			if (IsClickedBounds())
			{
				if (!isClickedLeft && Input::IsMouseButtonPressed(MouseButtonType::Left))
				{
					isClickedLeft = true;
					GameObject skillWindow = GetCurrentScene()->GetEntityFromTag("SkillWindow");
					skillWindow.SetActive(true);
				}
			}
			isClickedLeft = false;
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

