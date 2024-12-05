#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"
#include "PlayerStatus.hpp"

namespace fz {

	class TombScript : public VegaScript
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		Animator animator;
		AnimPool anims;
		Timer timer;
		GameObject player;
		bool genGhost = false;
		
	public:
		void Start() override
		{
			animator.SetTarget(GetCurrentEntity());
			anims["Fall"].loadFromFile("game/animations/player/tomb/fall.anim");
			anims["Land"].loadFromFile("game/animations/player/tomb/land.anim");
			auto& rigidComp = GetComponent<RigidbodyComponent>();
			rigidComp.SetGravityScale(2.0f);
			player = GetCurrentScene()->GetEntityFromTag("Player");
		}

		void OnDestroy() override
		{
		}

		void OnUpdate(float dt) override
		{
			animator.Update(dt);
			auto& boxComp = GetComponent<BoxCollider2DComponent>();

			const sf::Vector2f& currPos = GetWorldPosition();
			const sf::Vector2f& currSize = GetComponent<BoxCollider2DComponent>().GetHalfSize();
			const sf::Vector2f& playerPos = player.GetWorldPosition();
			const sf::Vector2f& playerSize = player.GetComponent<BoxCollider2DComponent>().GetHalfSize();
			if (currPos.y + currSize.y >= playerPos.y + playerSize.y)
			{
				auto& body = GetComponent<RigidbodyComponent>();
				animator.Play(&anims["Land"]);
				boxComp.SetTrigger(false);
				body.SetPosition({ playerPos.x, playerPos.y + playerSize.y });
				player.SetActive(false);

				if (!genGhost)
				{
					genGhost = true;
					GetCurrentScene()->Instantiate("Ghost", currPos);
				}
			}
			else
			{
				boxComp.SetTrigger(true);
				animator.Play(&anims["Fall"]);
			}
		}

	}; // class 
} // namespace fz
