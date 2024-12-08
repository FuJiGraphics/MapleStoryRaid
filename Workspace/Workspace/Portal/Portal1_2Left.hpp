#pragma once
#include <VegaEngine2.h>
#include "PortalComponent.hpp"
#include "SoundMgr.h"
namespace fz {

	class Portal1_2Left_Script : public VegaScript
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
			comp.NextPlayerPos = { 2508.f, 551.f };
			comp.NextScenePath = "game/scene/Stage1_town.vega";
			SOUND_MGR.PlayBgm("game/sound/Town_bgm.mp3", true);
		
	
		}

		void OnUpdate(float dt) override
		{
			animator.Update(dt);
		}
	};

} // namespace fz

