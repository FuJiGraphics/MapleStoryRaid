#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"

namespace fz {

	class IronhogScript : public VegaScript, public MonsterFSM
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		float JumpPower = -500.f;
		float MoveSpeed = 100.f;

		float KnockbackTime = 0.5f;

		Directions currDir = Directions::LEFT;

		Animator animator;
		AnimPool clips;

		GameObject CurrItemDrop;

		TransformComponent* transform;
		RigidbodyComponent* body;
		StatComponent* stat = nullptr;
		TransformComponent* targetTransform = nullptr;

		bool isOnDie = false;

		bool OnDropItem = false;
		Timer timer;

		void Start() override
		{
			transform = &GetComponent<TransformComponent>();
			body = &GetComponent<RigidbodyComponent>();
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			animator.SetTarget(GetCurrentEntity());
			animator.SetSpeed(1.0f);
			clips["idle"].loadFromFile("game/animations/ironhog_idle.anim");
			clips["move"].loadFromFile("game/animations/ironhog_move.anim");
			clips["damaged"].loadFromFile("game/animations/ironhog_damaged.anim");
			clips["die"].loadFromFile("game/animations/ironhog_die.anim");
			body->SetGravityScale(1.5f);
		}

		void OnDestroy() override
		{
			FZLOG_DEBUG("아이언호그 스크립트 파괴! {0} {1}", 1.1, "aSDAasdfSCA");
		}

		void OnUpdate(float dt) override
		{
			if (!HasComponent<RigidbodyComponent>())
				return;

			animator.Update(dt);
			timer.Update(dt);
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
			// 이동 적용
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
				animator.Play(&clips["move"]);;
			}
		}

		void Jump() override
		{
			// 바닥에 닿으면 점프 상태 해제
			if (body->IsOnGround())
			{
				body->AddPosition({ 0.0f, JumpPower });
			}
		}

		void Damaged(int damage) override
		{
			// 플레이어 피격시
			animator.Play(&clips["damaged"]);
		}

		void Die() override
		{
			animator.Play(&clips["die"]);
		}

	private:

	};

}



