#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"

namespace fz {

	class PlayerScript : public VegaScript, public CharacterFSM
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		float JumpPower = -500.f;
		float MoveSpeed = 100.f;

		float KnockbackTime = 0.5f;
		float AttackTime = 1.0f;

		Directions currDir = Directions::LEFT;

		Animator animator;
		AnimPool clips;

		TransformComponent* transform;
		RigidbodyComponent* body;

		Timer timer;
			
	public:
		void Start() override
		{
			transform = &GetComponent<TransformComponent>();
			body = &GetComponent<RigidbodyComponent>();
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			animator.SetTarget(sprite, *transform);
			clips["idle"].loadFromFile("game/animations/player_idle.anim");
			clips["idle"].Speed = 1.0f;
			clips["move"].loadFromFile("game/animations/player_move.anim");
			clips["move"].Speed = 1.0f;
			clips["jump"].loadFromFile("game/animations/player_jump.anim");
			clips["jump"].Speed = 1.0f;
			clips["damaged"].loadFromFile("game/animations/player_damaged.anim");
			clips["die"].loadFromFile("game/animations/player_die.anim");
			clips["attackFirst"].loadFromFile("game/animations/player_idle_attack_first.anim");
			clips["attackFirst"].Speed = 2.0f;
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
			timer.Update(dt);

			// 이동 적용
			if (Input::IsKeyPressed(KeyType::Right))
			{
				this->Move(Directions::RIGHT);
			}
			else if (Input::IsKeyPressed(KeyType::Left))
			{
				this->Move(Directions::LEFT);
			}
			else if (Input::IsKeyDown(KeyType::Q))
			{
				this->Damaged(0);
			}
			else if (Input::IsKeyPressed(KeyType::W))
			{
				this->Die();
			}
			else
			{
				this->Idle();
			}

			if (Input::IsKeyDown(KeyType::T))
			{
				GameObject mushmom = GetCurrentScene()->GetEntityFromTag("Mushmom");
				GetCurrentScene()->Instantiate(mushmom, { 200.f, 0.0f });
			}

			if (Input::IsKeyDown(KeyType::Space))
			{
				this->Jump();
			}
			if (Input::IsKeyDown(KeyType::LControl))
			{
				this->Attack();
			}
		}

		void Attack() override
		{
			if (!timer["Knocback"].Done())
				return;

			timer["Attack"].Start(AttackTime);
			animator.Play(&clips["attackFirst"]);
		}

		void Idle() override
		{
			if (!timer["Knocback"].Done())
				return;
			if (!timer["Attack"].Done())
				return;

			animator.Play(&clips["idle"]);
		}

		void Move(Directions dir) override
		{
			if (!timer["Knocback"].Done())
				return;
			if (!timer["Attack"].Done())
				return;

			fz::Transform& transform = GetComponent<TransformComponent>();
			if (dir == Directions::RIGHT)
			{
				body->AddPosition({ MoveSpeed * 1.f, 0.0f });
				transform.SetScale(-1.0f, 1.0f);
				animator.Play(&clips["move"]);
				currDir = Directions::RIGHT;
			}
			else if (dir == Directions::LEFT)
			{
				body->AddPosition({ MoveSpeed * -1.f, 0.0f });
				transform.SetScale(1.0f, 1.0f);
				animator.Play(&clips["move"]);
				currDir = Directions::LEFT;
			}
		}

		void Jump() override
		{
			if (!timer["Knocback"].Done())
				return;
			if (!timer["Attack"].Done())
				return;

			if (body->IsOnGround({0.0f, 0.34f}))
			{
				body->AddPosition({ 0.0f, JumpPower });
			}
		}

		void Damaged(int damage) override
		{
			animator.Play(&clips["damaged"]);
			if (currDir == Directions::LEFT)
				Knockback(Directions::RIGHT);
			else if (currDir == Directions::RIGHT)
				Knockback(Directions::LEFT);
		}

		void Die() override
		{
			// TODO: 죽음 이펙트 넣기
			// animator.Play(&clips["die"]);
		}

		void Knockback(Directions dir)
		{
			if (!timer["Knocback"].Done())
				return;

			SetColorWithChilds({ 100, 100, 100, 240 });
			timer["Knocback"].Start(KnockbackTime);
			if (dir == Directions::LEFT)
				body->AddForce({ -3000.f, -3000.0f });
			else if (dir == Directions::RIGHT)
				body->AddForce({ +3000.f, -3000.0f });
		}
	};
}
