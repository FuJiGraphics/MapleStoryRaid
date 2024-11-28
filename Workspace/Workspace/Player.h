#pragma once
#include <VegaEngine2.h>
#include "FSM.h"

namespace fz {

	class PlayerScript : public VegaScript, public CharacterFSM
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		float JumpPower = -500.f;
		float MoveSpeed = 100.f;

		Animator animator;
		AnimPool clips;

		TransformComponent* transform;
		RigidbodyComponent* body;

		void Start() override
		{
			transform = &GetComponent<TransformComponent>();
			body = &GetComponent<RigidbodyComponent>();
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			animator.SetTarget(sprite, *transform);
			animator.SetSpeed(1.0f);
			clips["idle"].loadFromFile("game/animations/player_idle.anim");
			clips["move"].loadFromFile("game/animations/player_move.anim");
			clips["jump"].loadFromFile("game/animations/player_jump.anim");
			clips["damaged"].loadFromFile("game/animations/player_damaged.anim");
			clips["die"].loadFromFile("game/animations/player_die.anim");
			body->SetGravityScale(1.5f);
		}

		void OnDestroy() override
		{
			FZLOG_DEBUG("플레이어 스크립트 소멸");
		}

		void OnUpdate(float dt) override
		{
			if (!HasComponent<RigidbodyComponent>())
				return;

			animator.Update(dt);

			// 이동 적용
			if (Input::IsKeyPressed(KeyType::D))
			{
				this->Move(Directions::RIGHT);
			}
			else if (Input::IsKeyPressed(KeyType::A))
			{
				this->Move(Directions::LEFT);
			}
			else if (Input::IsKeyPressed(KeyType::Q))
			{
				this->Damaged();
			}
			else if (Input::IsKeyPressed(KeyType::W))
			{
				this->Die();
			}

			else
			{
				this->Idle();
			}
			// 점프 처리
			if (Input::IsKeyDown(KeyType::Space))
			{
				this->Jump();
			}



		}

		void Idle() override
		{
			animator.Play(&clips["idle"]);
		}

		void Move(Directions dir) override
		{
			fz::Transform& transform = GetComponent<TransformComponent>();
			if (dir == Directions::RIGHT)
			{
				body->AddPosition({ MoveSpeed * 1.f, 0.0f });
				transform.SetScale(-1.0f, 1.0f);
				animator.Play(&clips["move"]);
			}
			else if (dir == Directions::LEFT)
			{
				body->AddPosition({ MoveSpeed * -1.f, 0.0f });
				transform.SetScale(1.0f, 1.0f);
				animator.Play(&clips["move"]);
			}
		}

		void Jump() override
		{
			if (body->IsOnGround({0.0f, 0.34f}))
			{
				body->AddPosition({ 0.0f, JumpPower });
			}
		}

		void Damaged() override
		{
			animator.Play(&clips["damaged"]);
		}

		void Die() override
		{
			// TODO: 죽음 이펙트 넣기
			// animator.Play(&clips["die"]);
		}
	};

}

BIND_SCRIPT(PlayerScript, "Player", PlayerScript);
