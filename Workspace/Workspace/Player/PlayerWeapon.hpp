#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Stat.hpp"

namespace fz {

	class PlayerWeaponScript : public VegaScript, public CharacterFSM
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		StatComponent* WeaponStat = nullptr;

		Animator animator;
		AnimPool clips;

		bool OnEffect = false;
		GameObject CurrEffect;
		fz::Entity parent;

		void Start() override
		{
			WeaponStat = &AddComponent<StatComponent>();
			WeaponStat->Stat.AD = 50;

			parent = GetComponent<ParentEntityComponent>().ParentEntity;
			clips["Idle"].loadFromFile("game/animations/weapon/basic_sword/idle.anim");
			clips["Move"].loadFromFile("game/animations/weapon/basic_sword/move.anim");
			clips["Jump"].loadFromFile("game/animations/weapon/basic_sword/jump.anim");
			clips["Attack1"].loadFromFile("game/animations/weapon/basic_sword/attack1.anim");
			TransformComponent& transform = GetComponent<TransformComponent>();
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			animator.SetTarget(GetCurrentEntity());
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
					if (animator.IsPlaying(2))
						Effect();
					break;
				case PlayerStatus::Damaged:
					animator.Stop();
					break;
			}
			if (status != PlayerStatus::Attack1)
			{
				OnEffect = false;
				if (CurrEffect)
				{
					GetCurrentScene()->DestroyInstance(CurrEffect);
				}
			}
		}

		void Effect()
		{
			if (!OnEffect)
			{
				OnEffect = true;
				const auto& scale = parent.GetComponent<TransformComponent>().Transform.GetScale();
				const auto& pos = GetWorldPosition();
				CurrEffect = GetCurrentScene()->Instantiate(
					"BasicAttack", { pos.x - (30.f * scale.x), pos.y - 25.f }, scale);
				auto& parentStat = parent.GetComponent<StatComponent>();
				auto& effectStat = CurrEffect.AddComponent<StatComponent>();
				effectStat.Stat.AD = WeaponStat->Stat.AD + parentStat.Stat.AD;
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

