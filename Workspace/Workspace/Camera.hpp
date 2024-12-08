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

		float leftBound = -1000.f;
		float rightBound = 1000.f;
		void Start() override
		{
			GameObject target = GetCurrentScene()->GetEntityFromTag("Player");
			
			const auto& targetPos = target.GetWorldPosition();
			auto& transform = GetComponent<TransformComponent>().Transform;
			transform.SetTranslate(targetPos);

			auto& body = GetComponent<RigidbodyComponent>();
			body.SetGravityScale(0.0f);

			GameObject line1 = GetCurrentScene()->GetEntityFromTag("CameraBlock1");
			const auto& lineTransform1 = line1.GetComponent<TransformComponent>().Transform;
			leftBound = lineTransform1.GetTranslate().x;
			GameObject line2 = GetCurrentScene()->GetEntityFromTag("CameraBlock2");
			const auto& lineTransform2 = line2.GetComponent<TransformComponent>().Transform;
			rightBound = lineTransform2.GetTranslate().x;
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

			if (newPos.x <= leftBound)
			{
				newPos.x = leftBound; // 왼쪽 경계에 도달하면 고정
			}
			else if (newPos.x >= rightBound)
			{
				newPos.x = rightBound; // 오른쪽 경계에 도달하면 고정
			}

			prevPos = { currPos.x, currPos.y };
			body.AddPositionNoGravity({ newPos.x - currPos.x, newPos.y - currPos.y });
		}

		void OnTriggerEnter(Collider collider) override
		{
			if (collider.tag == "B2")
				IsCollideBlockL = true;
			if (collider.tag == "B1")
				IsCollideBlockR = true;
		}

		void OnTriggerExit(Collider collider) override
		{
			if (collider.tag == "B2")
				IsCollideBlockL = false;
			if (collider.tag == "B1")
				IsCollideBlockR = false;
		}
	};
} // namespace fz

