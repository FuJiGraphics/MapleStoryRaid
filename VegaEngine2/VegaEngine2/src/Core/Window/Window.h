#pragma once
#include "Core/Core.h"

namespace fz {

	// Forwards
	class Event;
	class LayerStack;
	class RenderContext;

	enum class WindowAPI
	{
		SFML,
		GLFW,
	};

	struct WindowMode
	{
		std::string		Title;
		unsigned int	Width;
		unsigned int	Height;
		bool			VSync;
		WindowAPI		Api;
		unsigned int	AntialiasingLevel;
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(fz::Event&)>;

	protected:
		Window(Window&) = delete;
		Window& operator=(Window&) = delete;

	public:
		static std::shared_ptr<Window> Create(const WindowMode& mode);

		virtual void Init() = 0;
		virtual void Release() = 0;

		virtual void OnEvent() = 0;
		virtual void OnUpdate() = 0;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual std::string GetTitle() const = 0;
		virtual void* GetNativeWindow() const = 0;
		virtual HWND GetHandle() const = 0;
		virtual bool IsFocus() const = 0;
		virtual bool IsVSync() const = 0;
		virtual bool IsOpen() const = 0;

		virtual void SetTitle(const std::string& title) = 0;
		virtual void ActivateOpenGL(bool enabled) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

	protected:
		Window() = default;
		virtual ~Window() = default;
	};

} // namespace fz