#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"
#include "PlayerStatus.hpp"
#include "Stat.hpp"
#include "SaveData.hpp"

namespace fz {

	class PlayerScript : public VegaScript, public CharacterFSM
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		float LadderSpeed = 100.f;
		float RopeSpeed = 100.f;

		float KnockbackTime = 0.5f;
		float AttackTime = 1.0f;

		Directions currDir = Directions::LEFT;

		TransformComponent* transform = nullptr;
		RigidbodyComponent* body = nullptr;
		PlayerStatusComponent* status = nullptr;
		StatComponent* Stat = nullptr;

		bool isDead = false;

		bool isOnGround = false;
		bool isOnLadder = false;
		bool isOnRope = false;
	
		bool isInsideRope = false;
		bool isInsideLadder = false;
		bool isInsidePortal = false;

		float currGroundPosY = 0.0f;

		sf::FloatRect currLadderRect;
		sf::Vector2f currRopeBounds[2];

		GameObject TargetPortal;

		Timer timer;
			
	public:
		void Start() override
		{
			status = &AddComponent<PlayerStatusComponent>();
			Stat = &AddComponent<StatComponent>();
			transform = &GetComponent<TransformComponent>();
			body = &GetComponent<RigidbodyComponent>();
			body->SetGravityScale(1.5f);
			if (SaveData::ChangedScene)
			{
				SaveData::Get(*Stat);
				transform->Transform.SetTranslate(SaveData::Position);
				body->SetPosition(SaveData::Position);
			}
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

			isOnGround = body->IsOnGround();

			// 이동 적용
			if (Input::IsKeyDown(KeyType::Space))
			{
				if (isOnGround)
					this->Jump();
			}

			if (!isOnGround)
			{
				if (isOnLadder && isInsideLadder)
					this->OnLadder(Directions::NONE);
				else if (isOnRope && isInsideRope)
					this->OnRope(Directions::NONE);
				else
					this->Jump();
			}
			else
			{
				this->Idle();
			}
			
			if (Input::IsKeyPressed(KeyType::Right))
			{
				if (!isOnRope && !isOnLadder)
					this->Move(Directions::RIGHT);
			}
			else if (Input::IsKeyPressed(KeyType::Left))
			{
				if (!isOnRope && !isOnLadder)
					this->Move(Directions::LEFT);
			}
			else if (Input::IsKeyPressed(KeyType::Up))
			{
				if (isInsideLadder)
					this->OnLadder(Directions::UP);
				else if (isInsideRope)
					this->OnRope(Directions::UP);
				else if (isInsidePortal)
					this->ChangeScene();
			}
			else if (Input::IsKeyPressed(KeyType::Down))
			{
				if (isInsideLadder)
					this->OnLadder(Directions::DOWN);
				else if (isInsideRope)
					this->OnRope(Directions::DOWN);
			}
			else if (Input::IsKeyDown(KeyType::Q))
			{
				this->Damaged(10);
			}
			else if (Input::IsKeyDown(KeyType::T))
			{
				GetCurrentScene()->Instantiate("Balrog", { 600.f, 700.0f });
			}
			else if (Input::IsKeyDown(KeyType::W))
			{
				this->Die();
			}
			if (Input::IsKeyDown(KeyType::LControl))
			{
				if (!isOnRope && !isOnLadder)
					this->Attack();
			}
		}

		void OnTriggerEnter(Collider collider)override 
		{
			if (collider.tag == "Ladder")
			{
				isInsideLadder = true;
			}
		}

		void OnTriggerStay(Collider collider) override 
		{

		}
		void OnTriggerExit(Collider collider) override 
		{
			if (collider.tag == "Ladder")
			{
				isInsideLadder = false;
			}
		}

		void OnCollisionEnter(Collision collision) override
		{
			if (collision.gameObject.HasComponent<PortalComponent>())
			{
				isInsidePortal = true;
				TargetPortal = collision.gameObject;
			}
			else if (collision.tag == "Ladder")
			{
				isInsideLadder = true;
				const sf::Vector2f& pos = collision.gameObject.GetWorldPosition();
				const sf::Vector2f& half = collision.gameObject.GetComponent<BoxCollider2DComponent>().GetHalfSize();
				currLadderRect.left = pos.x - half.x;
				currLadderRect.top = pos.y - half.y;
				currLadderRect.width = pos.x + half.x;
				currLadderRect.height = pos.y + half.y;
			}
			else if (collision.tag == "Rope")
			{
				isInsideRope = true;
				const sf::Vector2f& pos = collision.gameObject.GetWorldPosition();
				const sf::Vector2f& startPos = collision.gameObject.GetComponent<EdgeCollider2DComponent>().GetStartPos();
				const sf::Vector2f& endPos = collision.gameObject.GetComponent<EdgeCollider2DComponent>().GetEndPos();
				currRopeBounds[0].y = pos.y;
				currRopeBounds[1].y = pos.y + (endPos.x - startPos.x);
			}
			else if (collision.tag == "Block")
			{

			}
		}

		void OnCollisionStay(Collision collision) override
		{
			if (collision.tag == "Ground")
			{
				
			}
		}

		void OnCollisionExit(Collision collision) override
		{
			if (collision.gameObject.HasComponent<PortalComponent>())
			{
				isInsidePortal = false;
				TargetPortal = {};
			}
			else if (collision.tag == "Ladder")
			{
				isInsideLadder = false;
			}
			else if (collision.tag == "Rope")
			{
				isInsideRope = false;
			}
			else if (collision.tag == "Block")
			{

			}
		}

		void Attack() override
		{
			if (!timer["Knocback"].Done())
				return;

			timer["Attack"].Start(AttackTime);
			status->Status = PlayerStatus::SwingAttack2;
			Effect();
		}

		void Idle() override
		{
			if (!timer["Knocback"].Done())
				return;
			if (!timer["Attack"].Done())
				return;
			auto& body = GetComponent<RigidbodyComponent>();
			body.SetGravityScale(1.5f);
			auto& collider = GetComponent<BoxCollider2DComponent>();
			collider.SetTrigger(false);
			isOnLadder = false;
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

			auto& stat = GetComponent<StatComponent>().Stat;
			fz::Transform& transform = GetComponent<TransformComponent>();
			if (dir == Directions::RIGHT)
			{
				RaycastHit hit;
				sf::Vector2f boxSize = GetComponent<BoxCollider2DComponent>().GetHalfSize();
				sf::Vector2f origin = GetWorldPosition();
				origin.y = origin.y + boxSize.y + 1.f;
				Physics.Raycast(origin, { 1.f, 0.f }, hit, boxSize.x + 5.f);
				sf::Vector2f nextDir = hit.Normal;
				// 벽이 아닐 경우
				if (hit.Collider.tag != "Block")
				{
					nextDir.x = nextDir.x * -1.f;
					if (Utils::IsEqual(nextDir.y, 0.f))
						body->AddPosition({ (float)stat.MoveSpeed, 0.f });
					else
						body->AddPosition(nextDir * (float)stat.MoveSpeed);
					transform.SetScale(-1.0f, 1.0f);
					currDir = Directions::RIGHT;
				}
			}
			else if (dir == Directions::LEFT)
			{
				RaycastHit hit;
				sf::Vector2f boxSize = GetComponent<BoxCollider2DComponent>().GetHalfSize();
				sf::Vector2f origin = GetWorldPosition();
				origin.y = origin.y + boxSize.y + 1.f;
				Physics.Raycast(origin, { -1.0f, 0.0f }, hit, boxSize.x + 5.f);
				sf::Vector2f nextDir = hit.Normal;
				// 벽일 경우
				if (hit.Collider.tag != "Block")
				{
					nextDir.x = nextDir.x * -1.f;
					if (Utils::IsEqual(nextDir.y, 0.f))
						body->AddPosition({ stat.MoveSpeed * -1.f, 0.f });
					else
						body->AddPosition(nextDir * ((float)stat.MoveSpeed) * 1.f);
					transform.SetScale(1.0f, 1.0f);
					currDir = Directions::LEFT;
				}
			}
		}

		void Jump() override
		{
			if (!timer["Knocback"].Done())
				return;
			if (!timer["Attack"].Done())
				return;

			status->Status = PlayerStatus::Jump;
			if (isOnGround)
			{
				auto& stat = GetComponent<StatComponent>().Stat;
				body->AddPosition({ 0.0f, stat.JumpPower * -1.f });
			}
		}

		void Damaged(int damage) override
		{
			if (isDead)
				return;

			Stat->Stat.CurrentHP -= damage;
			if (Stat->Stat.CurrentHP <= 0)
			{
				this->Die();
				return;
			}
			status->Status = PlayerStatus::Damaged;
			if (currDir == Directions::LEFT)
				Knockback(Directions::RIGHT);
			else if (currDir == Directions::RIGHT)
				Knockback(Directions::LEFT);
		}

		void Die() override
		{
			if (isDead)
				return;

			const sf::Vector2f& currPos = GetWorldPosition();
			GetCurrentScene()->Instantiate("Tomb", { currPos.x, currPos.y - 400.f });
			isDead = true;
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

		void ChangeScene()
		{
			SaveData::ChangedScene = true;
			SaveData::Set(*Stat);
			auto& comp = TargetPortal.GetComponent<PortalComponent>();
			SaveData::Position = comp.NextPlayerPos;
			SceneManager::RuntimeChangeScene(comp.NextScenePath);
		}

		void OnLadder(Directions dir)
		{
			if (!isInsideLadder)
				return;
			auto& body = GetComponent<RigidbodyComponent>();
			auto& collider = GetComponent<BoxCollider2DComponent>();
			const auto& pos = GetComponent<TransformComponent>().Transform.GetTranslate();
			const auto& half = collider.GetHalfSize();
			collider.SetTrigger(true);
			body.SetGravityScale(0.0f);
			isOnLadder = true;
			switch (dir)
			{
				case Directions::NONE:
					status->Status = PlayerStatus::Stop;
					body.AddPositionNoGravity({ 0.0f, 0.0f });
					break;
				case Directions::UP:
					if (pos.y - half.y > currLadderRect.top)
					{
						status->Status = PlayerStatus::Ladder;
						body.AddPositionNoGravity({ 0.0f, LadderSpeed * -1.f });
					}
					else
					{
						isOnLadder = false;
						collider.SetTrigger(false);
						body.SetGravityScale(1.5f);
					}
					break;
				case Directions::DOWN:
					if (pos.y + half.y < currLadderRect.height)
					{
						status->Status = PlayerStatus::Ladder;
						body.AddPositionNoGravity({ 0.0f, LadderSpeed * 1.f });
					}
					else
					{
						isOnLadder = false;
						collider.SetTrigger(false);
						body.SetGravityScale(1.5f);
					}
					break;
			}
		}

		void OnRope(Directions dir)
		{
			if (!isInsideRope)
				return;
			auto& body = GetComponent<RigidbodyComponent>();
			auto& collider = GetComponent<BoxCollider2DComponent>();
			const auto& pos = GetComponent<TransformComponent>().Transform.GetTranslate();
			const auto& half = collider.GetHalfSize();
			collider.SetTrigger(true);
			body.SetGravityScale(0.0f);
			isOnRope = true;
			switch (dir)
			{
				case Directions::NONE:
					status->Status = PlayerStatus::Stop;
					body.AddPositionNoGravity({ 0.0f, 0.0f });
					break;
				case Directions::UP:
					if (pos.y - half.y > currRopeBounds[0].y)
					{
						status->Status = PlayerStatus::Rope;
						body.AddPositionNoGravity({ 0.0f, RopeSpeed * -1.f });
					}
					else
					{
						isOnRope = false;
						collider.SetTrigger(false);
						body.SetGravityScale(1.5f);
					}
					break;
				case Directions::DOWN:
					if (pos.y + half.y < currRopeBounds[1].y)
					{
						status->Status = PlayerStatus::Rope;
						body.AddPositionNoGravity({ 0.0f, RopeSpeed * 1.f });
					}
					else
					{
						isOnRope = false;
						collider.SetTrigger(false);
						body.SetGravityScale(1.5f);
					}
					break;
			}
		}


		void Effect()
		{
			const auto& scale = GetComponent<TransformComponent>().Transform.GetScale();
			const auto& pos = GetWorldPosition();
			GameObject CurrEffect = GetCurrentScene()->Instantiate(
				"BasicAttack", { pos.x - (30.f * scale.x), pos.y - 25.f }, scale);
			auto& effectStat = CurrEffect.AddComponent<StatComponent>();
		}
	}; // class
} // namespace fz
