#pragma once
#include <VegaEngine2.h>
#include "FSM.h"

namespace fz {

	class PlayerWeaponScript : public VegaScript, public CharacterFSM
	{
	public:
		float Speed = 2000.f;

		void Start() override
		{

		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{

		}

		void OnTriggerEnter(Collider collider) override
		{

		}

		void OnTriggerStay(Collider collider) override
		{

		}

		void OnTriggerExit(Collider collider) override
		{

		}
	};
} // namespace fz

