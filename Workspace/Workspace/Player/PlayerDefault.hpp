#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"
#include "PlayerStatus.hpp"

namespace fz {

	class PlayerDefaultScript : public VegaScript, public CharacterFSM
	{
		using AnimPool = std::unordered_map<std::string, AnimationClip>;
	public:
		Animator animator;
		AnimPool anims;
		Timer timer;
		GameObject parent;

	public:
		void Start() override
		{
			parent = GetComponent<ParentEntityComponent>().ParentEntity;
			animator.SetTarget(GetCurrentEntity());
			anims["idle"].loadFromFile("game/animations/player/default/idle.anim");
			anims["walk"].loadFromFile("game/animations/player/default/walk.anim");
			anims["jump"].loadFromFile("game/animations/player/default/jump.anim");
			anims["ladder"].loadFromFile("game/animations/player/default/ladder.anim");
			anims["prone"].loadFromFile("game/animations/player/default/prone.anim");
			anims["prone_attack"].loadFromFile("game/animations/player/default/prone_attack.anim");
			anims["rope"].loadFromFile("game/animations/player/default/rope.anim");
			anims["alert"].loadFromFile("game/animations/player/default/alert.anim");
			anims["attack_dash_sting"].loadFromFile("game/animations/player/default/attack_dash_sting.anim");
			anims["attack_sting1"].loadFromFile("game/animations/player/default/attack_sting1.anim");
			anims["attack_sting2"].loadFromFile("game/animations/player/default/attack_sting2.anim");
			anims["attack_swing1"].loadFromFile("game/animations/player/default/attack_swing1.anim");
			anims["attack_swing2"].loadFromFile("game/animations/player/default/attack_swing2.anim");
			anims["attack_swing3"].loadFromFile("game/animations/player/default/attack_swing3.anim");
			anims["attack_swing4"].loadFromFile("game/animations/player/default/attack_swing4.anim");
		}

		void OnDestroy() override
		{
		}

		void OnUpdate(float dt) override
		{
			if (!parent.HasComponent<PlayerStatusComponent>())
				return;
			const auto& status = parent.GetComponent<PlayerStatusComponent>().Status;
			switch (status)
			{
				case PlayerStatus::Idle:
					animator.Play(&anims["idle"]);
					break;
				case PlayerStatus::Move:
					animator.Play(&anims["walk"]);
					break;
				case PlayerStatus::Jump:
					animator.Play(&anims["jump"]);
					break;
				case PlayerStatus::Ladder:
					animator.Play(&anims["ladder"]);
					break;
				case PlayerStatus::Rope:
					animator.Play(&anims["rope"]);
					break;
				case PlayerStatus::StingAttack1:
					animator.Play(&anims["attack_sting1"]);
					break;
				case PlayerStatus::StingAttack2:
					animator.Play(&anims["attack_sting2"]);
					break;
				case PlayerStatus::SwingAttack1:
					animator.Play(&anims["attack_swing1"]);
					break;
				case PlayerStatus::SwingAttack2:
					animator.Play(&anims["attack_swing2"]);
					break;
				case PlayerStatus::SwingAttack3:
					animator.Play(&anims["attack_swing3"]);
					break;
				case PlayerStatus::SwingAttack4:
					animator.Play(&anims["attack_swing4"]);
					break;
				case PlayerStatus::Damaged:
					// animator.Play(&anims["Walk"]);
					break;
				case PlayerStatus::Die:
					animator.Play(&anims["Walk"]);
					break;
				case PlayerStatus::Stop:
					animator.Stop();
			}
			animator.Update(dt);
		}

	}; // class 
} // namespace fz
