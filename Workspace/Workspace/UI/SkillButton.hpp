#pragma once
#include <VegaEngine2.h>
#include "BoundComponent.hpp"
#include "CallbackComponent.h"
#include "SkillButtonComponent.h"

namespace fz {

	class SkillButtonScript : public VegaScript
	{
	public:
		void Start() override
		{
			AddComponent<CallbackComponent>();
			AddComponent<SkillButtonComponent>();
		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			if (Input::IsMouseButtonPressed(MouseButtonType::Left))
			{
				auto& transform = GetComponent<TransformComponent>().Transform;
				const auto& mousePos = GetCurrentScene()->GetWorldMousePos();
				transform.SetTranslate(mousePos);
			}
			else if (Input::IsMouseButtonReleased(MouseButtonType::Left))
			{
				auto& callback = GetComponent<CallbackComponent>().Callbacks;
				auto& list = callback["Slots"];
				for (auto& fn : list)
				{
					fn(GetCurrentEntity());
				}
				GetCurrentEntity().SetActive(false);
			}
		}

		void OnTriggerEnter(Collider collider) override
		{
			FZLOG_DEBUG("Collider {0}", collider.tag);
		}

		void OnTriggerExit(Collider collider) override 
		{

		}

	};
} // namespace fz

