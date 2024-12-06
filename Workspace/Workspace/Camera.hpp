#pragma once
#include <VegaEngine2.h>
#include "FSM.h"

namespace fz {

	class CameraScript : public VegaScript
	{
	public:
		float Speed = 2000.f;
		bool IsCollideBlockL = false;
		bool IsCollideBlockR = false;
		sf::Vector2f prevPos;

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
			auto& body = GetComponent<RigidbodyComponent>();

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

			sf::Vector2f viewSize = GetComponent<CameraComponent>().Camera.GetSize();
			RaycastHit hitLeft;
			RaycastHit hitRight;
			Physics.Raycast(newPos, { -1.0f, 0.0f }, hitLeft, viewSize.x * 0.5f);
			Physics.Raycast(newPos, { 1.0f, 0.0f }, hitRight, viewSize.x * 0.5f);

			body.AddPositionNoGravity({ newPos.x - currPos.x, newPos.y - currPos.y });
		}
	};
} // namespace fz

