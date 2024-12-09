#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"
#include <random>
#include "CallbackComponent.h"

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

		GameObject CurrItemDrop;

		TransformComponent* transform = nullptr;
		RigidbodyComponent* body = nullptr;
		StatComponent* stat = nullptr;
		TransformComponent* targetTransform = nullptr;
		
		bool isOnDie = false;
		bool OnDropItem = false;

		Timer timer;

		enum class AIState { Idle, Moving, Chasing, Die } currentState = AIState::Idle;

		void Start() override
		{

			stat = &AddComponent<StatComponent>();
			stat->Stat.HP = 200;
			stat->Stat.MP = 0;

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

			timer["ActionTimer"].Start(5.0f); 
		}

		void OnDestroy() override
		{
			FZLOG_DEBUG("스포아 스크립트 파괴{0} { 1 }", 1.1, "aSDASCAsad");
		}

		void OnUpdate(float dt) override
		{
			if (!HasComponent<RigidbodyComponent>())
				return;

			auto& tagComp = GetComponent<TagComponent>();

			animator.Update(dt);
			timer.Update(dt);

			if (currentState == AIState::Die)
			{
				Die(); // Die 상태 지속 처리
				if (timer["Die"].Done())
				{
					GetCurrentScene()->DestroyInstance(GetCurrentEntity());
					DropItem();
				}
				return;
			}

			if (timer["DamagedCooldown"].IsStart() && timer["DamagedCooldown"].Done())
			{
				currentState = AIState::Chasing;
			}
			if (currentState == AIState::Chasing)
			{
				FollowTarget(dt);
				return;
			}

			if (!timer["Die"].IsStart() && timer["ActionTimer"].Done())
			{
				
				if (currentState == AIState::Moving)
				{
					currentState = AIState::Idle;
					timer["ActionTimer"].Start(3.0f); 
				}
				else
				{
					currentState = AIState::Moving;
					timer["ActionTimer"].Start(5.0f); 
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

		void DropItem()
		{
			//if (!OnDropItem)
			//{
			//	OnDropItem = true;
			//	const auto& scale = GetComponent<TransformComponent>().Transform.GetScale();
			//	const auto& pos = GetWorldPosition();
			//	CurrItemDrop = GetCurrentScene()->Instantiate(
			//		"MonsterItem", { pos.x - (30.f * scale.x), pos.y - 25.f }, scale);
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
			stat->Stat.HP -= damage;
			if (stat->Stat.HP <= 0)
			{
				stat->Stat.HP = 0;
				Die();
			}
			else
			{
				animator.Play(&clips["damaged"]);

				GameObject player = GetCurrentScene()->GetEntityFromTag("Player");
				targetPlayer = player;  

				if (currDir == Directions::LEFT)
					Knockback(Directions::RIGHT);
				else if (currDir == Directions::RIGHT)
					Knockback(Directions::LEFT);

				currentState = AIState::Chasing;
				currentState = AIState::Idle;
				timer["DamagedCooldown"].Start(1.0f);
			}
		}

		void Die() override
		{
			if (!timer["Die"].IsStart())
			{
				animator.Play(&clips["die"]);
				currentState = AIState::Die;
				timer["Die"].Start(1.f);
			}
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

		virtual void OnCollisionEnter(Collision collision)
		{
			if (collision.gameObject.HasComponent<SkillComponent>())
			{
				targetTransform = &collision.gameObject.GetComponent<TransformComponent>();

				const auto& stat = collision.gameObject.GetComponent<StatComponent>();
				Damaged(stat.Stat.AttackPower);
			}
		}

		void FollowTarget(float dt)
		{
			GameObject player = GetCurrentScene()->GetEntityFromTag("Player");
			auto targetPosition = player.GetComponent<TransformComponent>().Transform.GetTranslate();
			auto myPosition = transform->Transform.GetTranslate();

			Directions dir = (targetPosition.x > myPosition.x) ? Directions::RIGHT : Directions::LEFT;
			float moveAmount = MoveSpeed * dt;

			if (dir == Directions::RIGHT)
			{
				body->AddPosition({ MoveSpeed * 1, 0.0f });
				transform->Transform.SetScale(-1.0f, 1.0f);
				animator.Play(&clips["move"]);
			}
			else
			{
				body->AddPosition({ -MoveSpeed * 1, 0.0f });
				transform->Transform.SetScale(1.0f, 1.0f);
				animator.Play(&clips["move"]);
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

		GameObject targetPlayer;
	};

}
