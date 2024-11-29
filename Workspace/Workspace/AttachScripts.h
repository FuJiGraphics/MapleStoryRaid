#pragma once

// Ã¶Áø
#include "Camera.hpp"
#include "Player.h"

// ¹ÎÀç
#include "RibbonPig.h"
#include "Spoa.h"
#include "Snail.h"
#include "Stump.h"
#include "Fireboar.h"
#include "Ironhog.h"
#include "Mushmom.h"
#include "Eregos.h"


namespace fz {

	static void AttachScripts(Shared<Scene>& scene)
	{
		BIND_SCRIPT("Camera", CameraScript);
		BIND_SCRIPT("Player", PlayerScript);

		BIND_SCRIPT("Fireboar", FireboarScript);
		BIND_SCRIPT("Ironhog", IronhogScript);
		BIND_SCRIPT("RibbonPig", RibbonPigScript);
		BIND_SCRIPT("Snail", SnailScript);
		BIND_SCRIPT("Spoa", SpoaScript);
		BIND_SCRIPT("Stump", StumpScript);
		BIND_SCRIPT("Mushmom", MushmomScript);
		BIND_SCRIPT("Eregos", EregosScript);
	}

} // namespace fz
