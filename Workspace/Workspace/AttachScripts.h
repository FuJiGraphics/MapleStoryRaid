#pragma once

// Camera
#include "Camera.hpp"
// UI
#include "UI/UI.hpp"
#include "UI/CashShop.hpp"
#include "UI/StatUI.hpp"
#include "UI/StatWindow.hpp"
#include "UI/StatWindowExit.hpp"

// Effects
#include "Skill/BasicAttack.hpp"
// Portal
#include "Portal/Portal1R.hpp"
#include "Portal/Portal1_2Left.hpp"
// Player
#include "Player/Player.hpp"
#include "Player/PlayerDefault.hpp"

// πŒ¿Á
#include "RibbonPig.h"
#include "Spoa.h"
#include "Snail.h"
#include "Stump.h"
#include "Fireboar.h"
#include "Ironhog.h"
#include "Mushmom.h"
#include "Balrog.h"

namespace fz {

	static void AttachScripts(Shared<Scene>& scene)
	{
		BIND_SCRIPT("UI", UIScript);
		BIND_SCRIPT("CashShop", CashShopScript);
		BIND_SCRIPT("StatUI", StatUIScript);
		BIND_SCRIPT("StatWindow", StatWindowScript);
		BIND_SCRIPT("StatWindowExit", StatWindowExitScript);

		BIND_SCRIPT("BasicAttack", BasicAttackScript);
		BIND_SCRIPT("Portal1R", Portal1R_Script);
		BIND_SCRIPT("Portal1_2Left", Portal1_2Left_Script);
		BIND_SCRIPT("Camera", CameraScript);

		// Player
		BIND_SCRIPT("Player", PlayerScript);
		BIND_SCRIPT("PlayerDefault", PlayerDefaultScript);

		BIND_SCRIPT("Fireboar", FireboarScript);
		BIND_SCRIPT("Ironhog", IronhogScript);
		BIND_SCRIPT("RibbonPig", RibbonPigScript);
		BIND_SCRIPT("Snail", SnailScript);
		BIND_SCRIPT("Spoa", SpoaScript);
		BIND_SCRIPT("Stump", StumpScript);
		BIND_SCRIPT("Mushmom", MushmomScript);
		BIND_SCRIPT("balrog", BalrogScript);
	}

} // namespace fz
