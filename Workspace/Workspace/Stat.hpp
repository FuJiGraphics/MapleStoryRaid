#pragma once
#include <VegaEngine2.h>

namespace fz {

	struct Stat
	{
		std::string Name = "None";
		std::string Class = "Hero";
		std::string Guild = "Kyungil";
		int Level = 15;				// 레벨
		int Experience = 0;			// 경험치
		int Popularity = 0;			// 인기도
		int HP = 100;				// 최대 체력
		int CurrentHP = HP;			// 현재 체력
		int MP = 100;				// 마나
		int CurrentMP = MP;			// 현재 마나
		int AttackPower = 10;		// 공격력
		int PhysicalDefense = 0;	// 물리 방어력
		int MagicPower = 10;		// 마력
		int MagicDefense = 0;		// 마법 방어력
		int Accuracy = 80;			// 명중률
		int Evasion = 0;			// 회피율
		int Dexterity = 0;			// 손재주
		int MoveSpeed = 100;		// 이동 속도
		int JumpPower = 500;		// 점프력

		int NumOfStr = 0;
		int NumOfDex = 0;
		int NumOfInt = 0;
		int NumOfLuk = 0;

		void ApplySTR(int STR) 
		{
			NumOfStr += STR;
			AttackPower += STR * 2;    // 예시로 STR에 따라 공격력 증가
			PhysicalDefense += STR * 1; // STR에 따른 물리 방어력 증가
			Accuracy += STR * 1;        // STR에 따른 명중률 증가
		}

		void ApplyDEX(int DEX)
		{
			NumOfDex += DEX;
			Accuracy += DEX * 1;        // DEX에 따른 명중률 증가
			Evasion += DEX * 1;         // DEX에 따른 회피율 증가
			Dexterity += DEX * 2;       // DEX에 따른 손재주 증가
			MoveSpeed += DEX * 1;       // DEX에 따른 이동 속도 증가
		}

		void ApplyINT(int INT)
		{
			NumOfInt += INT;
			MagicPower += INT * 3;      // INT에 따른 마력 증가
			MagicDefense += INT * 2;    // INT에 따른 마법 방어력 증가
		}

		void ApplyLUK(int LUK)
		{
			NumOfLuk += LUK;
			Evasion += LUK * 2;         // LUK에 따른 회피율 증가
			JumpPower += LUK * 1;       // LUK에 따른 점프력 증가
		}
	};

	struct StatComponent
	{
	public:
		Stat Stat;

		StatComponent() = default;
		StatComponent(const StatComponent&) = default;
	};
} // namespace fz