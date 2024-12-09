#pragma once
#include "Stat.hpp"
#include "UI/Slot/SlotComponent.h"

namespace fz {

	class SaveData
	{
	public:
		SaveData() = default;
		~SaveData() = default;

		inline static bool ChangedScene = false;
		inline static sf::Vector2f Position = { 0.0f, 0.0f };
		inline static Stat Status;
		inline static std::unordered_map<std::string, SlotComponent> Slots;

		static void Set(const StatComponent& stat)
		{
			auto& src = stat.Stat;
			Status.HP = src.HP;
			Status.CurrentHP = src.CurrentHP;
			Status.MP = src.MP;
			Status.CurrentMP = src.CurrentMP;
			Status.AttackPower = src.AttackPower;
			Status.PhysicalDefense = src.PhysicalDefense;
			Status.MagicPower = src.MagicPower;
			Status.MagicDefense = src.MagicDefense;
			Status.Accuracy = src.Accuracy;
			Status.Evasion = src.Evasion;
			Status.Dexterity = src.Dexterity;
			Status.MoveSpeed = src.MoveSpeed;
			Status.JumpPower = src.JumpPower;

			Status.NumOfStr = src.NumOfStr;
			Status.NumOfDex = src.NumOfDex;
			Status.NumOfInt = src.NumOfInt;
			Status.NumOfLuk = src.NumOfLuk;
		}

		static void Set(const std::string& tag, const SlotComponent& slot)
		{
			Slots[tag].IsMounted = slot.IsMounted;
			Slots[tag].Key = slot.Key;
			Slots[tag].SkillTag = slot.SkillTag;
			Slots[tag].Tag = slot.Tag;
			Slots[tag].SkillTexturePath = slot.SkillTexturePath;
		}

		static void Get(StatComponent& stat)
		{
			auto& dst = stat.Stat;
			dst.HP = Status.HP;
			dst.CurrentHP = Status.CurrentHP;
			dst.MP = Status.MP;
			dst.CurrentMP = Status.CurrentMP;
			dst.AttackPower = Status.AttackPower;
			dst.PhysicalDefense = Status.PhysicalDefense;
			dst.MagicPower = Status.MagicPower;
			dst.MagicDefense = Status.MagicDefense;
			dst.Accuracy = Status.Accuracy;
			dst.Evasion = Status.Evasion;
			dst.Dexterity = Status.Dexterity;
			dst.MoveSpeed = Status.MoveSpeed;
			dst.JumpPower = Status.JumpPower;

			dst.NumOfStr = Status.NumOfStr;
			dst.NumOfDex = Status.NumOfDex;
			dst.NumOfInt = Status.NumOfInt;
			dst.NumOfLuk = Status.NumOfLuk;
		}

		static void Get(const std::string& tag, SlotComponent& slot)
		{
			slot.IsMounted = Slots[tag].IsMounted;
			slot.Key = Slots[tag].Key;
			slot.SkillTag = Slots[tag].SkillTag;
			slot.Tag = Slots[tag].Tag;
			slot.SkillTexturePath = Slots[tag].SkillTexturePath;
		}
	};

}