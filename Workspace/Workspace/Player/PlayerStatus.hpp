#pragma once
#include <VegaEngine2.h>

namespace fz {

	enum class PlayerStatus
	{
		Idle,
		Move,
		Jump,
		Alert,
		Rope,
		Ladder,
		Prone,
		ProneAttack,
		DashStingAttack,
		StingAttack1,
		StingAttack2,
		SwingAttack1,
		SwingAttack2,
		SwingAttack3,
		SwingAttack4,
		Damaged, 
		Die,
		Stop,
	};

	struct PlayerStatusComponent
	{
	public:
		PlayerStatus Status = PlayerStatus::Idle;
		bool IsLoginMode = false;

		PlayerStatusComponent() = default;
		PlayerStatusComponent(const PlayerStatusComponent&) = default;
	};


}