#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"
#include "PlayerStatus.hpp"

namespace fz {

	class GhostScript : public VegaScript
	{
	public:
		GameObject player;
		float speed = 100.f;
		float radius = 20.0f;
		fz::Transform transform;
		float angle = 0;
		sf::Vector2f centor;

	public:
		void Start() override
		{
			auto& body = GetComponent<RigidbodyComponent>();
			player = GetCurrentScene()->GetEntityFromTag("Player");
			const auto& playerPos = player.GetWorldPosition();
			body.SetPosition({ playerPos.x, playerPos.y - 100.f });
			body.SetGravityScale(0.0f);
			centor = { playerPos.x - 5.f, playerPos.y - 15.f };
		}

		void OnDestroy() override
		{
		}
		 
		void OnUpdate(float dt) override
		{
			auto& body = GetComponent<RigidbodyComponent>();
			angle = Utils::Clamp(angle + (speed * dt), 0.0f, 360.f);
			angle = angle >= 360.f ? 0.0f : angle;
			const sf::Vector2f dir = Utils::Angle(angle);
			body.SetPosition(centor + (dir * radius));
		}

	}; // class 
} // namespace fz
