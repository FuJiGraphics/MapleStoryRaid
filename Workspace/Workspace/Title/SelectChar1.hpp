#pragma once
#include <VegaEngine2.h>
#include "Stat.hpp"

namespace fz {

	class SelectChar1Script : public VegaScript
	{
	public:
		Animator animator;
		AnimationClip clip;

		void Start() override
		{
			animator.SetTarget(GetCurrentEntity());
			clip.loadFromFile("game/animations/ui/SelectChar.anim");
			animator.Play(&clip);
		}
		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			animator.Update(dt);
		}
	};
} // namespace fz
