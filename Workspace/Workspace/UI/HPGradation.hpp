#pragma once
#include <VegaEngine2.h>
#include "Stat.hpp"

namespace fz {

	class HPGradationScript : public VegaScript
	{
	public:
		GameObject target;
		Stat* stat;
		Transform* transform;

		void Start() override
		{
			target = GetCurrentScene()->GetEntityFromTag("Player");
			stat = &target.GetComponent<StatComponent>().Stat;
			transform = &GetComponent<TransformComponent>().Transform;
		}
		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			float t = (float)stat->CurrentHP / stat->HP;
			float scaleX = Utils::Lerp(106.f, 1.0f, t);
			float scaleY = transform->GetScale().y;
			transform->SetScale(scaleX, scaleY);
		}
	};
} // namespace fz
