#include "pch.h"
#include "Window.h"
#include "Platform/SFML/Window/WindowSFML.h"

namespace fz {

	std::shared_ptr<Window> Window::Create(const WindowMode& mode)
	{
		switch (mode.Api)
		{
			case WindowAPI::SFML:
				return std::make_shared<WindowSFML>(mode);
				break;
			case WindowAPI::GLFW:
				return nullptr;
				break;
		}
		return nullptr;
	}

} // namespace fz