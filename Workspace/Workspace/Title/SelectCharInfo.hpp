#pragma once
#include <VegaEngine2.h>
#include "Stat.hpp"
#include "Utils/Timer.h"
#include "InfoComponent.h"

namespace fz {

#define UpdateStatFromString(statType, str) statMap[#statType].GetComponent<TextComponent>().Text.setString(str);
#define UpdateStatFromInt(statType, num) statMap[#statType].GetComponent<TextComponent>().Text.setString(std::to_string(num));

	class SelectCharInfoScript : public VegaScript
	{
	public:
		std::unordered_map<std::string, fz::Entity> statMap;
		bool IsFirstInit = true;

		void Start() override
		{
			auto& infoComp = AddComponent<InfoComponent>();
			infoComp.entity = GetCurrentEntity();
			auto& childComp = GetComponent<ChildEntityComponent>();
			infoComp.animator.SetTarget(infoComp.entity);
			infoComp.clip.loadFromFile("game/animations/ui/OpenCharInfo.anim");
			Utils::SetChildTagsToMap(statMap, childComp);
			infoComp.map = &statMap;
		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			auto& infoComp = GetComponent<InfoComponent>();
			if (GetCurrentEntity().GetActive())
				infoComp.Init();
			infoComp.animator.Update(dt);
			infoComp.timer.Update(dt);

			if (infoComp.timer["SelectInfo"].IsStart() && 
				infoComp.timer["SelectInfo"].Done())
			{
				this->UpdateStatus("Player");
			}
		}

		void UpdateStatus(const std::string& tag)
		{
			GameObject target = GetCurrentScene()->GetEntityFromTag(tag);
			if (target)
			{
				if (target.HasComponent<StatComponent>())
				{
					auto& stat = target.GetComponent<StatComponent>().Stat;
					UpdateStatFromString(LoginInfoRanking, "Ranking not available");
					UpdateStatFromString(LoginInfoClass, stat.Class);
					UpdateStatFromInt(LoginInfoLevel, stat.Level);
					UpdateStatFromInt(LoginInfoSTR, stat.NumOfStr);
					UpdateStatFromInt(LoginInfoDEX, stat.NumOfDex);
					UpdateStatFromInt(LoginInfoINT, stat.NumOfInt);
					UpdateStatFromInt(LoginInfoLUK, stat.NumOfLuk);
					UpdateStatFromInt(LoginInfoPOP, stat.Popularity);
				}
				else
				{
					FZLOG_WARN("타겟의 Status를 찾을 수 없습니다.");
				}
			}
			else
			{
				FZLOG_WARN("타겟 오브젝트를 찾을 수 없습니다.");
			}
		}

	};
} // namespace fz
