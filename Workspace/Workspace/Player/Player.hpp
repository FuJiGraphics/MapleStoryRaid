#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"
#include "PlayerStatus.hpp"

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

		TransformComponent* transform;
		RigidbodyComponent* body;
		PlayerStatusComponent* status;

		bool isOnGround = false;

		Timer timer;
			
	public:
		void Start() override
		{
			status = &AddComponent<PlayerStatusComponent>();
			transform = &GetComponent<TransformComponent>();
			body = &GetComponent<RigidbodyComponent>();
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

			timer.Update(dt);

			isOnGround = body->IsOnGround({ 0.0f, 0.34f });

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
		}

		void Idle() override
		{
			if (!timer["Knocback"].Done())
				return;
			if (!timer["Attack"].Done())
				return;
			if (!body->IsOnGround({ 0.0f, 0.34f }))
				return;

			status->Status = PlayerStatus::Idle;
			SetColorWithChilds({ 255, 255, 255, 255 });
		}

		void Move(Directions dir) override
		{
			if (!timer["Knocback"].Done())
				return;
			if (!timer["Attack"].Done())
				return;

			if (isOnGround)
			{
				status->Status = PlayerStatus::Move;
			}

			fz::Transform& transform = GetComponent<TransformComponent>();
			if (dir == Directions::RIGHT)
			{
				body->AddPosition({ MoveSpeed * 1.f, 0.0f });
				transform.SetScale(-1.0f, 1.0f);
				currDir = Directions::RIGHT;
			}
			else if (dir == Directions::LEFT)
			{
				body->AddPosition({ MoveSpeed * -1.f, 0.0f });
				transform.SetScale(1.0f, 1.0f);
				currDir = Directions::LEFT;
			}
		}

		void Jump() override
		{
			if (!timer["Knocback"].Done())
				return;
			if (!timer["Attack"].Done())
				return;

			if (isOnGround)
			{
				status->Status = PlayerStatus::Jump;
				body->AddPosition({ 0.0f, JumpPower });
			}
		}

		void Damaged(int damage) override
		{
			status->Status = PlayerStatus::Damaged;
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
