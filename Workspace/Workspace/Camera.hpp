#pragma once
#include <VegaEngine2.h>
#include "FSM.h"

namespace fz {

	class CameraScript : public VegaScript
	{
	public:
		float Speed = 2000.f;

		void Start() override
		{
			GameObject target = GetCurrentScene()->GetEntityFromTag("Player");
			
			const auto& targetPos = target.GetWorldPosition();
			auto& transform = GetComponent<TransformComponent>().Transform;
			transform.SetTranslate(targetPos);

			auto& body = GetComponent<RigidbodyComponent>();
			body.SetGravityScale(0.0f);
		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			GameObject target = GetCurrentScene()->GetEntityFromTag("Player");
			auto& transform = target.GetComponent<TransformComponent>();
			const auto& targetPos = transform.Transform.GetTranslate();
			sf::Vector2f currPos = GetWorldPosition();

			float moveSpeed = Speed * dt;
			sf::Vector2f newPos = Utils::Lerp(currPos, targetPos, moveSpeed);

			if (std::abs(targetPos.y - currPos.y) < 30.f)
			{
				newPos.y = currPos.y;
			}
			auto& body = GetComponent<RigidbodyComponent>();
			body.AddPositionNoGravity({ newPos.x - currPos.x, newPos.y - currPos.y });
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

