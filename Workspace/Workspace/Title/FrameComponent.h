#pragma once

#include <VegaEngine2.h>

namespace fz {

	enum class FrameStatus
	{
		Login, CharSelect,
		LoginDone, CharSelectDone,
		LoginExit, CharSelectExit,
	};

	struct FrameComponent
	{
		FrameStatus status = FrameStatus::Login;

		FrameComponent() = default;
		FrameComponent(const FrameComponent&) = default;
	};

}