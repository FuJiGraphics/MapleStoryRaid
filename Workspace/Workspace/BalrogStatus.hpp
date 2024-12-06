#pragma once
#include <VegaEngine2.h>
namespace fz {

	enum class BalrogStatus
	{
		Idle, Move,
		Skill1, Skill2, Skill3,
		Damaged, Die
	};

	struct BalrogStatusComponent
	{
	public:
		BalrogStatus Status = BalrogStatus::Idle;

		BalrogStatusComponent() = default;
		BalrogStatusComponent(const BalrogStatusComponent&) = default;
	};
}