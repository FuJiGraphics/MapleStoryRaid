#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "SkillComponent.hpp"
#include "Utils/Timer.h"

namespace fz {

	class StrikeAttackScript : public VegaScript, public CharacterFSM
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		Animator animator;
		AnimPool clips;
		Timer timer;

		void Start() override
		{
			auto& skillComp = AddComponent<SkillComponent>();

			clips["Attack"].loadFromFile("game/animations/effect/strike/strike.anim");
			TransformComponent& transform = GetComponent<TransformComponent>();
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			animator.SetTarget(GetCurrentEntity());
			timer["Attack"].Start(0.8f);
			auto& stat = AddComponent<StatComponent>();
			stat.Stat.AttackPower = 150.f;
		}

		void OnDestroy() override
		{
			animator.Stop();
		}

		void OnUpdate(float dt) override
		{
			timer.Update(dt);

			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			const sf::Texture* texture = sprite.getTexture();
			animator.Play(&clips["Attack"]);
			animator.Update(dt);
			if (timer["Attack"].Done())
				GetCurrentScene()->DestroyInstance(GetCurrentEntity());
		}
	};


} // namespace fz

