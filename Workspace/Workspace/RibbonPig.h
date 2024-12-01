#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"
#include <random>

namespace fz {

	class RibbonPigScript : public VegaScript, public MonsterFSM
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		float JumpPower = -500.f;
		float MoveSpeed = 100.f;

		float KnockbackTime = 0.5f;

		Directions currDir = Directions::LEFT;

		Animator animator;
		AnimPool clips;

		TransformComponent* transform;
		RigidbodyComponent* body;

		Timer timer;

		enum class AIState { Idle, Moving } currentState = AIState::Moving;
		void Start() override
		{
			transform = &GetComponent<TransformComponent>();
			body = &GetComponent<RigidbodyComponent>();
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			animator.SetTarget(GetCurrentEntity());
			animator.SetSpeed(1.0f);
			clips["idle"].loadFromFile("game/animations/ribbon_pig_idle.anim");
			clips["move"].loadFromFile("game/animations/ribbon_pig_move.anim");
			clips["damaged"].loadFromFile("game/animations/ribbon_pig_damaged.anim");
			clips["die"].loadFromFile("game/animations/ribbon_pig_die.anim");
			body->SetGravityScale(1.5f);

			timer["ActionTimer"].Start(5.0f); // �̵� ���·� ����
		}

		void OnDestroy() override
		{
			FZLOG_DEBUG("�������� ��ũ��Ʈ �ı�! {0} {1}", 1.1, "aSDASCA");
		}

		void OnUpdate(float dt) override
		{
			if (!HasComponent<RigidbodyComponent>())
				return;

			animator.Update(dt);
			timer.Update(dt);

			if (timer["ActionTimer"].Done())
			{
				// ���� ��ȯ
				if (currentState == AIState::Moving)
				{
					currentState = AIState::Idle;
					timer["ActionTimer"].Start(3.0f); // Idle ���� �ð�
				}
				else
				{
					currentState = AIState::Moving;
					timer["ActionTimer"].Start(5.0f); // �̵� ���� �ð�
					MoveDirection = GetRandomDirection();
				}
			}
			if (currentState == AIState::Moving)
			{
				Move(MoveDirection, dt);
			}
			else if (Input::IsKeyPressed(KeyType::Q))
			{
				this->Damaged(0);
			}
			else
			{
				Idle();
			}
			//// ���� ó��
			//if (Input::IsKeyDown(KeyType::Space))
			//{
 		//		this->Jump();
			//}
		}

		void Idle() override
		{
			if (!timer["Knocback"].Done())
				return;

			animator.Play(&clips["idle"]);
		}

		void Move(Directions dir, float dt)
		{
			if (!timer["Knocback"].Done())
				return;

			fz::Transform& transform = GetComponent<TransformComponent>();
			// �̵� ����
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
			if (!timer["Knocback"].Done())
				return;
			// �ٴڿ� ������ ���� ���� ����
			if (body->IsOnGround({ 0.0f, 0.4f }))
			{
				body->AddPosition({ 0.0f, JumpPower });
			}
		}

		void Damaged(int damage) override
		{
			// �÷��̾� �ǰݽ�
			animator.Play(&clips["damaged"]);
			if (currDir == Directions::LEFT)
				Knockback(Directions::RIGHT);
			else if (currDir == Directions::RIGHT)
				Knockback(Directions::LEFT);
		}

		void Die() override
		{
			animator.Play(&clips["die"]);

		}
		void Knockback(Directions dir)
		{
			if (!timer["Knocback"].Done())
				return;
			timer["Knocback"].Start(KnockbackTime);
			if (dir == Directions::LEFT)
				body->AddForce({ -1000.f, -1000.0f });
			else if (dir == Directions::RIGHT)
				body->AddForce({ +1000.f, -1000.0f });
		}
	private:
		Directions MoveDirection = Directions::RIGHT;

		Directions GetRandomDirection()
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(0, 1);
			return dis(gen) == 0 ? Directions::LEFT : Directions::RIGHT;
		}
	};

}




