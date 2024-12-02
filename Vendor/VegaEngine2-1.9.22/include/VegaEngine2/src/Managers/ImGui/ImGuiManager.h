#pragma once

#include "Core/Core.h"
#include "Core/Window/Window.h"
#include "imgui.h"

namespace sf {
	class Event;
	class Time;
}

namespace fz {
	
	class ImGuiManager
	{
	public:
		static bool Init(const fz::Window& window);
		static void Release();
		static void	PollEvent(const sf::Event& ev);
		static void	SetWindow(const fz::Window& window);
		static void SetDocking(bool enabled);
		static void	Begin(const sf::Time& dt);
		static bool IsFocusedWindow();
		static void ActivateInputAll(bool enabled);
		static void ActivateInputKeyboard(bool enabled);
		static void ActivateInputMouse(bool enabled);
		static void	End();
		static void	ShowDemo();

		static void SetDarkTheme();

	private:
		static const fz::Window* s_currWindow;
	};

} // namespace fz