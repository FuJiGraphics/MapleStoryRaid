#pragma once

#include <VegaEngine2.h>
#include "Utils/Timer.h"

namespace fz {

	struct InfoComponent
	{
		GameObject entity;
		Timer timer;
		Animator animator;
		AnimationClip clip;
		std::unordered_map<std::string, fz::Entity>* map;
		bool IsFirstStart = true;

		void Init()
		{
			if (!IsFirstStart)
				return;
			IsFirstStart = false;
			this->animator.Play(&clip);
			this->timer["SelectInfo"].Start(1.0f);
		}

		void Delete()
		{
			IsFirstStart = true;
			this->animator.Stop();
			this->timer["SelectInfo"].Reset();
			for (auto& entity : *map)
			{
				entity.second.GetComponent<TextComponent>().Text.setString("");
			}
		}

		InfoComponent() = default;
		InfoComponent(const InfoComponent&) = default;
	};

}