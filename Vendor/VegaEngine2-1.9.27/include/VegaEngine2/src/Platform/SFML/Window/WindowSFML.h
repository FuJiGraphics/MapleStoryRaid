#pragma once
#include "Core/Window/Window.h"

// forwards
namespace sf {
	class Window;
	class RenderWindow;
}

namespace fz {

	class WindowSFML : public Window
	{
	protected:
		WindowSFML(WindowSFML&) = delete;
		WindowSFML& operator=(WindowSFML&) = delete;

	public:
		WindowSFML(const fz::WindowMode& mode);
		virtual ~WindowSFML();

		void Init() override;
		void Release() override;

		void OnEvent() override;
		void OnUpdate() override;

		int GetWidth() const override;
		int GetHeight() const override;
		std::string GetTitle() const override;
		void* GetNativeWindow() const override;
		HWND GetHandle() const override;
		bool IsFocus() const override;
		bool IsVSync() const override;
		bool IsOpen() const override;

		void SetTitle(const std::string& title) override;
		void ActivateOpenGL(bool enabled) override;
		void SetVSync(bool enabled) override; 
		void SetEventCallback(const EventCallbackFn& callback) override;
	
	protected:
		Unique<sf::RenderWindow>		m_Window;
		fz::WindowMode					m_Mode;
		EventCallbackFn					m_CallbackFn;
		bool							m_IsVSync;
	};

} // namespace fz