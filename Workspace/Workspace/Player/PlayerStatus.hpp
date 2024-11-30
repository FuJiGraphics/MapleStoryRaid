#pragma once
#include <VegaEngine2.h>

namespace fz {

	enum class PlayerStatus
	{
		Idle, Move, Jump, 
		Attack1, Attack2, Attack3,
		Damaged, Die
	};

	struct PlayerStatusComponent
	{
	public:
		PlayerStatus Status = PlayerStatus::Idle;

		PlayerStatusComponent() = default;
		PlayerStatusComponent(const PlayerStatusComponent&) = default;
	};


}