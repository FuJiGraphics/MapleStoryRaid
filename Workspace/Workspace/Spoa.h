#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"
#include <random>

namespace fz {

	class SpoaScript : public VegaScript, public MonsterFSM
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
		
		bool isOnDie = false;

		Timer timer;

		enum class AIState { Idle, Moving, Die } currentState = AIState::Moving;

		void Start() override
		{
			auto& stat = AddComponent<StatComponent>();
			stat.Stat.HP = 200;
			stat.Stat.MP = 0;
			stat.Stat.AD = 10;
			stat.Stat.AP = 0;

			transform = &GetComponent<TransformComponent>();
			body = &GetComponent<RigidbodyComponent>();

			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			animator.SetTarget(GetCurrentEntity());
			animator.SetSpeed(1.0f);
			clips["idle"].loadFromFile("game/animations/spoa_idle.anim");
			clips["move"].loadFromFile("game/animations/spoa_move.anim");
			clips["damaged"].loadFromFile("game/animations/spoa_damaged.anim");
			clips["die"].loadFromFile("game/animations/spoa_die.anim");

			body->SetGravityScale(1.5f); 

			timer["ActionTimer"].Start(5.0f); // 이동 상태로 시작
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
			timer.Update(dt);

			if (timer["Die"].IsStart() && timer["Die"].Done())
			{
				GetCurrentScene()->DestroyInstance(GetCurrentEntity());
				return;
			}
			if (Input::IsKeyDown(KeyType::Y))
				this->Damaged(50);

			if (!timer["Die"].IsStart() && timer["ActionTimer"].Done())
			{
				// 상태 전환
				if (currentState == AIState::Moving)
				{
					currentState = AIState::Idle;
					timer["ActionTimer"].Start(3.0f); // Idle 지속 시간
				}
				else
				{
					currentState = AIState::Moving;
					timer["ActionTimer"].Start(5.0f); // 이동 지속 시간
					MoveDirection = GetRandomDirection();
				}
			}
			if (currentState == AIState::Moving)
			{
				Move(MoveDirection, dt);
			}
			else if (currentState == AIState::Die)
			{
				Die();
			}
			else
			{
				Idle();
			}

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
			// 이동 적용
			currDir = dir;
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

		void Damaged(int damage) override
		{
			auto& stat = GetComponent<StatComponent>();
			stat.Stat.HP -= damage;
			if (stat.Stat.HP <= 0)
			{
				stat.Stat.HP = 0;
				Die();
			}
			else
			{
				animator.Play(&clips["damaged"]);
				if (currDir == Directions::LEFT)
					Knockback(Directions::RIGHT);
				else if (currDir == Directions::RIGHT)
					Knockback(Directions::LEFT);
			}
		}

		void Die() override
		{
			auto& stat = GetComponent<StatComponent>();
			animator.Play(&clips["die"]);
			currentState = AIState::Die;
			timer["Die"].Start(0.5f);
			stat.Stat.IsDead = true;
		}

		void Knockback(Directions dir)
		{
			if (!timer["Knocback"].Done())
				return;
			timer["Knocback"].Start(KnockbackTime);
			if (dir == Directions::LEFT)
				body->AddForce({ -2000.f, -2000.0f });
			else if (dir == Directions::RIGHT)
				body->AddForce({ +2000.f, -2000.0f });
		}


		virtual void OnCollisionEnter(Collision collision)
		{
			if (collision.gameObject.HasComponent<SkillComponent>())
			{
				const auto& stat = collision.gameObject.GetComponent<StatComponent>();
				Damaged(stat.Stat.AD);
			}
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
