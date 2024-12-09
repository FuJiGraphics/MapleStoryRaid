#pragma once
#include <VegaEngine2.h>
#include "CallbackComponent.h"
#include "SlotComponent.h"
#include "UI/SkillButtonComponent.h"
#include "Player/SaveData.hpp"

namespace fz {

	class SlotT4Script : public VegaScript
	{
	public:
		std::string Tag;
		bool IsFirstUpdate = true;

		void Start() override
		{
			auto& slotComp = AddComponent<SlotComponent>();
			Tag = GetComponent<TagComponent>().Tag;
			slotComp.Tag = Tag;
			slotComp.Key = KeyType::PageUp;
			if (SaveData::ChangedScene)
			{
				SaveData::Get(Tag, slotComp);
			}
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
				callbacks["Slots"].push_back(BIND_EVENT_FUNC(SlotT4Script::OnReleasedButton));
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
			slotComp.SkillTag = buttonComp.Skill.GetComponent<TagComponent>().Tag;
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
					const auto& targetTag = slot.GetComponent<SlotComponent>().SkillTag;
					if (skillTag == targetTag)
					{
						slot.GetComponent<SlotComponent>().IsMounted = false;
						slot.SetActive(false);
					}
				}
			}
		}
	};
} // namespace fz
