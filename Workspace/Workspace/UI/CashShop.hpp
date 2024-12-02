#pragma once
#include <VegaEngine2.h>
#include "FSM.h"

namespace fz {

	class CashShopScript : public VegaScript
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
			const auto& pos = GetWorldPosition();

			if (Input::IsMouseButtonPressed(MouseButtonType::Left))
			{
				const auto& mousePos = Input::GetMousePosition();
				FZLOG_DEBUG("click mouse {0}, {1}", mousePos.x, mousePos.y);
				FZLOG_DEBUG("pos {0}, {1}", pos.x, pos.y);
			}
		}

		void OnTriggerEnter(Collider collider) override
		{
		}

		void OnTriggerStay(Collider collider) override
		{

		}

		void OnTriggerExit(Collider collider) override
		{

		}
	};
} // namespace fz

