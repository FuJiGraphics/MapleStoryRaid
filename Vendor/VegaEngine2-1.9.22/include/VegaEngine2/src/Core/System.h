#pragma once

#include "Core.h"

namespace fz {

	class Window;
	class LayerStack;
	class LayerPool;
	class Layer;

	class System
	{
	protected:
		// Using
		using Super = fz::System;
		// Delete
		FZ_DELETE_COPY(System)

	public:
		System(int width, int height, const std::string& title);
		virtual ~System();

		void Init();
		bool Release();
		bool Reset();

		void Run();
		void OnEvent(fz::Event& e);
		
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		bool OnWindowClose(WindowCloseEvent e);
		bool OnWindowResize(WindowResizeEvent e);

		Shared<Scene> LoadScene(const std::string& path);

		inline void ExitSystem() { this->OnWindowClose({}); }
		inline bool IsRunning() const { return m_Window->IsOpen(); }
		inline Window& GetWindow() const { return *m_Window; }
		inline int GetWidth() const { return Width; }
		inline int GetHeight() const { return Height; }
		static System& GetSystem() { return *s_System; }

	private:
		bool GenerateWindow();
		bool GenerateLayerStack();
		bool ReleaseLayerStack();

	protected:
		bool				IsInit;
		int					Width;
		int					Height;
		std::string			Title;
		Shared<LayerPool>	LayerGenerator;

	private:
		bool					m_IsRunning;
		Shared<Window>			m_Window;
		Shared<LayerStack>		m_LayerStack;
		inline static System*	s_System = nullptr;
	};

#define FRAMEWORK System::GetSystem()

	/*
	* @brief 클라이언트에서 재정의하여 시스템을 생성합니다.
	* @return System*: 생성한 시스템의 주소
	*/
	System* CreateSystem();
}
