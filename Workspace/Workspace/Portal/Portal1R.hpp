#pragma once
#include <VegaEngine2.h>
#include "PortalComponent.hpp"
#include "SoundMgr.h"

namespace fz {

	class Portal1R_Script : public VegaScript
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		Animator animator;
		AnimPool clips;

		void Start() override
		{
			clips["Portal"].loadFromFile("game/animations/effect/portal/portal.anim");
			TransformComponent& transform = GetComponent<TransformComponent>();
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			animator.SetTarget(GetCurrentEntity());
			animator.Play(&clips["Portal"]);

			auto& comp = AddComponent<PortalComponent>();
			comp.NextPlayerPos = { -613.f, -290.f };
			comp.NextScenePath = "game/scene/Stage1_hunt.vega";

			SoundMgr::Instance().PlayBgm("game/sound/Hunt_bgm.mp3", true);
		
		}

		void OnUpdate(float dt) override
		{
			animator.Update(dt);

		}
	};

} // namespace fz

