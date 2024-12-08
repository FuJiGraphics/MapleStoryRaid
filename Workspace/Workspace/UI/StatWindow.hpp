#pragma once
#include <VegaEngine2.h>
#include "Stat.hpp"

namespace fz {

#define UpdateStatFromString(statType, str) statMap[#statType].GetComponent<TextComponent>().Text.setString(str);
#define UpdateStatFromInt(statType, num) statMap[#statType].GetComponent<TextComponent>().Text.setString(std::to_string(num));

	class StatWindowScript : public VegaScript
	{
	public:
		sf::Vector2f halfSize;
		sf::Vector2f prevMousePos;
		bool clicked = false;

		std::unordered_map<std::string, fz::Entity> statMap;

		void Start() override
		{
			halfSize = GetComponent<BoxCollider2DComponent>().GetHalfSize();
			auto& childComp = GetComponent<ChildEntityComponent>();
			Utils::SetChildTagsToMap(statMap, childComp);
		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			this->UpdateStatus("Player");
			if (IsClickedBounds())
			{
				if (Input::IsMouseButtonPressed(MouseButtonType::Left))
				{
					if (!clicked)
					{
						clicked = true;
						prevMousePos = GetCurrentScene()->GetWorldMousePos();
					}
					else
					{
						const sf::Vector2f& mousePos = GetCurrentScene()->GetWorldMousePos();
						if (prevMousePos != mousePos)
						{
							auto& transformComp = GetComponent<TransformComponent>();
							sf::Vector2f nextPos = mousePos - prevMousePos;
							const auto& worldInverse = GetWorldTransform().getInverse().getMatrix();
							sf::Vector2f scale = { worldInverse[0], worldInverse[5] };
							nextPos = { nextPos.x * scale.x, nextPos.y * scale.y };

							const auto& currPos = transformComp.Transform.GetTranslate();
							transformComp.Transform.SetTranslate(currPos + nextPos);
							prevMousePos = mousePos;
						}
					}
				}
				else
				{
					clicked = false;
				}
			}
			else
				clicked = false;
		}

		bool IsClickedBounds()
		{
			const sf::Vector2f& mousePos = GetCurrentScene()->GetWorldMousePos();
			const auto& pos = GetWorldPosition();
			const auto& scale = GetWorldTransform().getMatrix();
			sf::Vector2f bounds[2];
			bounds[0] = { pos.x - (halfSize.x * scale[0]), pos.y - (halfSize.y * scale[5]) };
			bounds[1] = { pos.x + (halfSize.x * scale[0]), pos.y + (halfSize.y * scale[5]) };
			if (bounds[0].x > mousePos.x || bounds[0].y > mousePos.y)
				return false;
			if (bounds[1].x < mousePos.x || bounds[1].y < mousePos.y)
				return false;
			return true;
		}

		void UpdateStatus(const std::string& tag)
		{
			GameObject target = GetCurrentScene()->GetEntityFromTag(tag);
			if (target)
			{
				if (target.HasComponent<StatComponent>())
				{
					auto& stat = target.GetComponent<StatComponent>().Stat;
					UpdateStatFromString(Name, stat.Name);
					UpdateStatFromString(Name, stat.Name);
					UpdateStatFromString(Class, stat.Class);
					UpdateStatFromString(Guild, stat.Guild);
					UpdateStatFromInt(Level, stat.Level);
					UpdateStatFromInt(HP, stat.HP);
					UpdateStatFromInt(MP, stat.MP);
					UpdateStatFromInt(STR, stat.NumOfStr);
					UpdateStatFromInt(DEX, stat.NumOfDex);
					UpdateStatFromInt(INT, stat.NumOfInt);
					UpdateStatFromInt(LUK, stat.NumOfLuk);
					UpdateStatFromInt(Experience, stat.Experience);
					UpdateStatFromInt(Popularity, stat.Popularity);
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

	}; // class
} // namespace fz

