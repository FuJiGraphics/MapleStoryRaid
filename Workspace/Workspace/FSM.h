#pragma once
#include <VegaEngine2.h>

namespace fz {

	enum class Directions
	{
		NONE, 
		LEFT, RIGHT, 
		UP, DOWN
	};

	class MonsterFSM
	{
	public:
		MonsterFSM() = default;
		virtual ~MonsterFSM() = default;

		virtual void Idle() {}
		virtual void Move(Directions dir) {}
		virtual void Jump() {}
		virtual void Attack() {}
		virtual void Damaged(int damage) {}
		virtual void Die() {}

		virtual void Skill1() {}
		virtual void Skill2() {}
		virtual void Skill3() {}
		virtual void Skill4() {}

		virtual void Spawn() {}
	};

	class CharacterFSM
	{
	public:
		CharacterFSM() = default;
		virtual ~CharacterFSM() = default;

		virtual void Idle() {}
		virtual void Move(Directions dir) {}
		virtual void Jump() {}
		virtual void Attack() {}
		virtual void Damaged(int damage) {}
		virtual void Die() {}

		virtual void Skill1() {}
		virtual void Skill2() {}
		virtual void Skill3() {}
		virtual void Skill4() {}
		virtual void Skill5() {}
		virtual void Skill6() {}
	};

} // namespace fz