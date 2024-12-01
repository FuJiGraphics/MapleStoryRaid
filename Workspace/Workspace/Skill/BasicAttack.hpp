#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "SkillComponent.hpp"

namespace fz {

	class BasicAttackScript : public VegaScript, public CharacterFSM
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		Animator animator;
		AnimPool clips;

		int i = 0;
		void Start() override
		{
			auto& skillComp = AddComponent<SkillComponent>();

			clips["Attack"].loadFromFile("game/animations/effect/basic_attack/attack.anim");
			TransformComponent& transform = GetComponent<TransformComponent>();
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			animator.SetTarget(GetCurrentEntity());
		}

		void OnDestroy() override
		{
			animator.Stop();
		}

		void OnUpdate(float dt) override
		{
			i++;
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			const sf::Texture* texture = sprite.getTexture();
			animator.Play(&clips["Attack"]);
			animator.Update(dt);
		}
	};


} // namespace fz

