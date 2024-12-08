#pragma once
#include <VegaEngine2.h>
#include "CallbackComponent.h"
#include "SlotComponent.h"
#include "../SkillButtonComponent.h"

namespace fz {

	class SlotB1Script : public VegaScript
	{
	public:
		std::string Tag;
		bool IsFirstUpdate = true;

		void Start() override
		{
			auto& slotComp = AddComponent<SlotComponent>();
			Tag = GetComponent<TagComponent>().Tag;
			slotComp.Tag = Tag;
		}

		void OnDestroy() override
		{
			IsFirstUpdate = true;
		}

		void OnUpdate(float dt) override
		{
			if (IsFirstUpdate)
			{
				IsFirstUpdate = false;
				GameObject button = GetCurrentScene()->GetEntityFromTag("SkillButton");
				auto& callbacks = button.GetComponent<CallbackComponent>().Callbacks;
				callbacks["Slots"].push_back(BIND_EVENT_FUNC(SlotB1Script::OnReleasedButton));
			}
		}

		void OnReleasedButton(GameObject button)
		{
			const sf::Vector2f& pos = GetWorldPosition();
			const sf::Vector2f& half = GetComponent<BoxCollider2DComponent>().GetHalfSize();
			const sf::Vector2f& mousePos = GetCurrentScene()->GetWorldMousePos();

			float left = pos.x - half.x;
			float top = pos.y - half.y;
			float width = pos.x + half.x;
			float height = pos.y + half.y;
			if (left > mousePos.x || top > mousePos.y)
				return;
			if (width < mousePos.x || height < mousePos.y)
				return;

			SlotComponent& slotComp = GetComponent<SlotComponent>();
			SkillButtonComponent& buttonComp = button.GetComponent<SkillButtonComponent>();
			const std::string& skillTag = buttonComp.Skill.GetComponent<TagComponent>().Tag;
			this->ClearSlots(skillTag);

			// Icon ¼³Á¤
			SetActive(true);
			slotComp.IsMounted = true;
			slotComp.Skill = buttonComp.Skill;
			auto& spriteComp = GetComponent<SpriteComponent>();
			spriteComp.Sprite.SetTexture(buttonComp.IconPath);
		}

		void ClearSlots(const std::string& skillTag)
		{
			std::vector<GameObject> list;
			GetCurrentScene()->GetEntitiesFromComponent<SlotComponent>(list);
			for (auto& slot : list)
			{
				if (slot != GetCurrentEntity())
				{
					auto& targetSkill = slot.GetComponent<SlotComponent>().Skill;
					if (!targetSkill)
						continue;
					const auto& targetTag = targetSkill.GetComponent<TagComponent>().Tag;
					if (skillTag == targetTag)
					{
						slot.GetComponent<SlotComponent>().IsMounted = false;
						targetSkill = {};
						slot.SetActive(false);
					}
				}
			}
		}
	};
} // namespace fz
