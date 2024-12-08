#pragma once
#include <VegaEngine2.h>
#include "FrameComponent.h"
#include "Player/PlayerStatus.hpp"
#include "InfoComponent.h"

namespace fz {

	class Char1Script : public VegaScript
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
					GameObject player = GetCurrentScene()->GetEntityFromTag("Player");
					player.GetComponent<PlayerStatusComponent>().Status = PlayerStatus::Move;
					GameObject selectEffect = GetCurrentScene()->GetEntityFromTag("SelectChar1");
					selectEffect.SetActive(true);
					GameObject selectInfo = GetCurrentScene()->GetEntityFromTag("CharInfoWindow");
					selectInfo.SetActive(true);
				}
			}
			else if (Input::IsMouseButtonPressed(MouseButtonType::Left))
			{
				GameObject player = GetCurrentScene()->GetEntityFromTag("Player");
				player.GetComponent<PlayerStatusComponent>().Status = PlayerStatus::Idle;
				GameObject selectEffect = GetCurrentScene()->GetEntityFromTag("SelectChar1");
				selectEffect.SetActive(false);
				GameObject selectInfo = GetCurrentScene()->GetEntityFromTag("CharInfoWindow");
				selectInfo.SetActive(false);
				selectInfo.GetComponent<InfoComponent>().Delete();
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

