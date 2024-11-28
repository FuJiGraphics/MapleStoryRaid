#pragma once
#include <VegaEngine2.h>

namespace fz {

	enum class Directions
	{
		LEFT, RIGHT
	};

	class MonsterFSM
	{
	public:
		MonsterFSM() = default;
		~MonsterFSM() = default;

		virtual void Idle() {}
		virtual void Move(Directions dir) {}
		virtual void Jump() {}
		virtual void Attack() {}
		virtual void Damaged() {}
		virtual void Die() {}

		virtual void Skill1() {}
		virtual void Skill2() {}
		virtual void Skill3() {}
		virtual void Skill4() {}
	};

} // namespace fz