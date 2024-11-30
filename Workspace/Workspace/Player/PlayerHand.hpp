#pragma once
#include <VegaEngine2.h>
#include "FSM.h"

namespace fz {

	class PlayerHandScript : public VegaScript, public CharacterFSM
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		Animator animator;
		AnimPool clips;

		fz::Entity parent;

		void Start() override
		{
			parent = GetComponent<ParentEntityComponent>().ParentEntity;
			clips["Idle"].loadFromFile("game/animations/player/hand/idle.anim");
			clips["Move"].loadFromFile("game/animations/player/hand/move.anim");
			clips["Jump"].loadFromFile("game/animations/player/hand/jump.anim");
			TransformComponent& transform = GetComponent<TransformComponent>();
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			animator.SetTarget(sprite, transform);
		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			animator.Update(dt);

			PlayerStatus status = parent.GetComponent<PlayerStatusComponent>().Status;
			switch (status)
			{
				case PlayerStatus::Idle:
					animator.Play(&clips["Idle"]);
					break;
				case PlayerStatus::Move:
					animator.Play(&clips["Move"]);
					break;
				case PlayerStatus::Jump:
					animator.Play(&clips["Jump"]);
					break;
				case PlayerStatus::Attack1:
					animator.Stop();
					break;
				case PlayerStatus::Damaged:
					animator.Stop();
					break;
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

