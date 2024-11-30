#pragma once
#include <VegaEngine2.h>
#include "FSM.h"

namespace fz {

	class PlayerBodyScript : public VegaScript, public CharacterFSM
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		Animator animator;
		AnimPool clips;

		fz::Entity parent;

		void Start() override
		{
			parent = GetComponent<ParentEntityComponent>().ParentEntity;

			clips["Idle"].loadFromFile("game/animations/player/body/idle.anim");
			clips["Move"].loadFromFile("game/animations/player/body/move.anim");
			clips["Jump"].loadFromFile("game/animations/player/body/jump.anim");
			clips["Damaged"].loadFromFile("game/animations/player/body/damaged.anim");
			clips["Die"].loadFromFile("game/animations/player/body/die.anim");
			clips["Attack1"].loadFromFile("game/animations/player/body/attack1.anim");

			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			TransformComponent& transform = GetComponent<TransformComponent>();
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
					animator.Play(&clips["Attack1"]);
					break;
				case PlayerStatus::Attack2:
					animator.Play(&clips["Attack1"]);
					break;
				case PlayerStatus::Attack3:
					animator.Play(&clips["Attack1"]);
					break;
				case PlayerStatus::Damaged:
					animator.Play(&clips["Damaged"]);
					break;
				case PlayerStatus::Die:
					animator.Play(&clips["Die"]);
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

