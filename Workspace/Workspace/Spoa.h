#pragma once
#include <VegaEngine2.h>
#include "FSM.h"

namespace fz {

	class Spoa : public VegaScript, public MonsterFSM
	{
	public:
		float JumpPower = -500.f;
		float MoveSpeed = 100.f;

		Animator animator;
		AnimationClip idle;
		AnimationClip move;
		AnimationClip damaged;
		AnimationClip die;

		TransformComponent* transform;
		RigidbodyComponent* body;

		void Start() override
		{
			transform = &GetComponent<TransformComponent>();
			body = &GetComponent<RigidbodyComponent>();
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			animator.SetTarget(sprite, *transform);
			animator.SetSpeed(1.0f);
			idle.loadFromFile("game/animations/spoa_idle.anim");
			move.loadFromFile("game/animations/spoa_move.anim");
			damaged.loadFromFile("game/animations/spoa_damaged.anim");
			die.loadFromFile("game/animations/spoa_die.anim");

			body->SetGravityScale(1.5f);
		

		}

		void OnDestroy() override
		{
			FZLOG_DEBUG("스포아 스크립트 파괴!{0} { 1 }", 1.1, "aSDASCAsad");
		}

		void OnUpdate(float dt) override
		{
			if (!HasComponent<RigidbodyComponent>())
				return;
			animator.Update(dt);


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

		}



		void Idle() override
		{
			animator.Play(&idle);
		}

		void Move(Directions dir) override
		{
			fz::Transform& transform = GetComponent<TransformComponent>();
			// 이동 적용
			if (dir == Directions::RIGHT)
			{
				body->AddPosition({ MoveSpeed * 1.f, 0.0f });
				transform.SetScale(-1.0f, 1.0f);
				animator.Play(&move);
			}
			else if (dir == Directions::LEFT)
			{
				body->AddPosition({ MoveSpeed * -1.f, 0.0f });
				transform.SetScale(1.0f, 1.0f);
				animator.Play(&move);
			}
		}

		void Damaged() override
		{
			// 플레이어 피격시
			animator.Play(&damaged);
		}

		void Die() override
		{
			animator.Play(&die);
		}
	private:

	};

}

BIND_SCRIPT(Spoa, "Spoa", Spoa);
