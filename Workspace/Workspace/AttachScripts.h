#pragma once

// Login
#include "Title/LoginBox.hpp"
#include "Title/TitleFrame.hpp"
#include "Title/Char1.hpp"
#include "Title/SelectChar1.hpp"
#include "Title/SelectCharInfo.hpp"
#include "Title/GameStartButton.hpp"
// Map
#include "Map/AmherstTown.hpp"
#include "Map/AmherstHunt.hpp"
#include "Map/AmherstBoss.hpp"
// Camera
#include "Camera.hpp"
// UI
#include "UI/UI.hpp"
#include "UI/HPGradation.hpp"
#include "UI/CashShop.hpp"
#include "UI/StatUI.hpp"
#include "UI/StatWindow.hpp"
#include "UI/StatWindowExit.hpp"
#include "UI/SkillUI.hpp"
#include "UI/SkillWindow.hpp"
#include "UI/SkillWindowExit.hpp"
#include "UI/SkillButton.hpp"
#include "UI/SkillBound1.hpp"
#include "UI/SkillBound4.hpp"
#include "UI/Slot/SlotT1.hpp"
#include "UI/Slot/SlotT2.hpp"
#include "UI/Slot/SlotT3.hpp"
#include "UI/Slot/SlotT4.hpp"
#include "UI/Slot/SlotB1.hpp"
#include "UI/Slot/SlotB2.hpp"
#include "UI/Slot/SlotB3.hpp"
#include "UI/Slot/SlotB4.hpp"
#include "UI/Fade.hpp"
// Effects
#include "Skill/BasicAttack.hpp"
#include "Skill/PowerStrike.hpp"
// Portal
#include "Portal/Portal1R.hpp"
#include "Portal/Portal1_2Left.hpp"
#include "Portal/Potal1R_hunt.hpp"
#include "Portal/Portal1L_Boss.hpp"
// Player
#include "Player/Player.hpp"
#include "Player/PlayerDefault.hpp"
#include "Player/Tomb.hpp"
#include "Player/Ghost.hpp"
// Spawn
#include "Spawn/SpawnB.hpp"
#include "Spawn/SpawnBoss.hpp"

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
		BIND_SCRIPT("TitleFrame", TitleFrameScript);
		BIND_SCRIPT("LoginButton", LoginButtonScript);
		BIND_SCRIPT("Char1", Char1Script);
		BIND_SCRIPT("SelectChar1", SelectChar1Script);
		BIND_SCRIPT("CharInfoWindow", SelectCharInfoScript);
		BIND_SCRIPT("GameStartButton", GameStartButtonScript);
		BIND_SCRIPT("Fade", FadeScript);

		BIND_SCRIPT("AmherstTown", AmherstTownScript);
		BIND_SCRIPT("AmherstHunt", AmherstHuntScript);
		BIND_SCRIPT("AmherstBoss", AmherstBossScript);

		BIND_SCRIPT("UI", UIScript);
		BIND_SCRIPT("HPGradation", HPGradationScript);
		BIND_SCRIPT("CashShop", CashShopScript);
		BIND_SCRIPT("StatUI", StatUIScript);
		BIND_SCRIPT("StatWindow", StatWindowScript);
		BIND_SCRIPT("StatWindowExit", StatWindowExitScript);
		BIND_SCRIPT("SkillUI", SkillUIScript);
		BIND_SCRIPT("SkillWindow", SkillWindowScript);
		BIND_SCRIPT("SkillWindowExit", SkillWindowExitScript);
		BIND_SCRIPT("SkillButton", SkillButtonScript);
		BIND_SCRIPT("SkillBound1", SkillBound1Script);
		BIND_SCRIPT("SkillBound4", SkillBound4Script);
		BIND_SCRIPT("SlotT1", SlotT1Script);
		BIND_SCRIPT("SlotT2", SlotT2Script);
		BIND_SCRIPT("SlotT3", SlotT3Script);
		BIND_SCRIPT("SlotT4", SlotT4Script);
		BIND_SCRIPT("SlotB1", SlotB1Script);
		BIND_SCRIPT("SlotB2", SlotB2Script);
		BIND_SCRIPT("SlotB3", SlotB3Script);
		BIND_SCRIPT("SlotB4", SlotB4Script);

		BIND_SCRIPT("BasicAttack", BasicAttackScript);
		BIND_SCRIPT("PowerStrike", StrikeAttackScript);
		BIND_SCRIPT("Portal1R", Portal1R_Script);
		BIND_SCRIPT("Portal1_2Left", Portal1_2Left_Script);
		BIND_SCRIPT("Portal1R_hunt", Portal1R_Hunt_Script);
		BIND_SCRIPT("Portal1L_Boss", Portal1L_Boss_Script);
		BIND_SCRIPT("Camera", CameraScript);
		// Spawn
		BIND_SCRIPT("SpawnB", SpawnBScript);
		BIND_SCRIPT("SpawnBoss", SpawnBossScript);
		// Player
		BIND_SCRIPT("Player", PlayerScript);
		BIND_SCRIPT("PlayerDefault", PlayerDefaultScript);
		BIND_SCRIPT("Tomb", TombScript);
		BIND_SCRIPT("Ghost", GhostScript);
			
		BIND_SCRIPT("Fireboar", FireboarScript);
		BIND_SCRIPT("Ironhog", IronhogScript);
		BIND_SCRIPT("RibbonPig", RibbonPigScript);
		BIND_SCRIPT("Snail", SnailScript);
		BIND_SCRIPT("Spoa", SpoaScript);
		BIND_SCRIPT("Stump", StumpScript);
		BIND_SCRIPT("Mushmom", MushmomScript);
		BIND_SCRIPT("Balrog", BalrogScript);
	}

} // namespace fz
