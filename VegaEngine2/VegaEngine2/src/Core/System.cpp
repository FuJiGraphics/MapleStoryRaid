#include "pch.h"
#include "System.h"

namespace fz { 

	System::System(int width, int height, const std::string& title)
		: IsInit(false)
		, Width(width)
		, Height(height)
		, Title(title)
		, LayerGenerator(nullptr)
		, m_LayerStack(nullptr)
		, m_Window(nullptr)
		, m_IsRunning(true)
	{
		this->Init();
		s_System = this;
	}

	System::~System()
	{
		this->Release();
	}

	void System::Init()
	{
		if (IsInit)
		{
			FZLOG_WARN("초기화를 할 수 없습니다. 이미 초기화된 System입니다.");
			return;
		}
		FZLOG_INFO("System 초기화 중");
		FZLOG_ASSERT((Width > 0 && Height > 0), "윈도우 생성 사이즈가 잘못되었습니다. 생성 인자: width = {0}, height = {1}", Width, Height);
		this->GenerateWindow();
		this->GenerateLayerStack();
		ImGuiManager::Init(*m_Window);
		InputManager::SetTargetTrackingWindow((sf::RenderWindow*)m_Window->GetNativeWindow());
		InputManager::Init();
		Renderer2D::Init((sf::RenderWindow*)m_Window->GetNativeWindow());
		IsInit = true;
		FZLOG_INFO("System 초기화 성공");
	}

	bool System::Release()
	{
		if (!IsInit)
			return false;

		bool result = true;
		FZLOG_INFO("System 해제");
		// Release LayerStack
		{
			this->ReleaseLayerStack();
		}
		// Window
		{ 
			if (m_Window->IsOpen())
				m_Window->Release();
		}
		// ImGui
		{
			ImGuiManager::Release();
		}
		if (result)
			FZLOG_INFO("System 해제 완료");
		else
			FZLOG_WARN("System 해제 실패");
		IsInit = !result;
		return result;
	}

	bool System::Reset()
	{
		bool result = true;
		FZLOG_INFO("System 리셋");
		this->ReleaseLayerStack();

		if (result)
			FZLOG_INFO("System 해제 완료");
		else
			FZLOG_WARN("System 해제 실패");
		return result;
	}

	void System::Run()
	{
		FZLOG_INFO("System Run...");
		sf::Clock clock;
		sf::Clock fpsClock;

		while (m_IsRunning && m_Window->IsOpen())
		{
			sf::Time t = clock.restart();
			float dt = t.asSeconds();

			float frameTimeMs = dt * 1000.0f; // 프레임 시간(ms)
			float fps = (dt > 0.0f) ? 1.0f / dt : 0.0f; // FPS

			if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
				const std::string& title = m_Window->GetTitle();
				m_Window->SetTitle(title + " - [Debug]" + 
								   std::to_string(static_cast<int>(fps)) + "FPS, " + 
								   std::to_string(static_cast<int>(frameTimeMs)) + "ms");
				fpsClock.restart(); // 1초 간격으로 제목 업데이트
			}

			// Event
			m_Window->OnEvent();
			if (!m_Window->IsOpen())
				continue;

			// Update
			for (Layer* obj : *m_LayerStack)
			{
				if (obj != nullptr)
				{
					obj->OnUpdate(dt);
				}
			}
			InputManager::Update(dt);

			// ImGui
			ImGuiManager::Begin(t);
			{
				for (Layer* obj : *m_LayerStack)
				{
					if (obj != nullptr)
					{
						obj->OnUI();
					}
				}
			}
			ImGuiManager::End();

			m_Window->OnUpdate();
		}
		FZLOG_INFO("System Run Off");
	}

	void System::OnEvent(fz::Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(System::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(System::OnWindowResize));
		for (Layer* obj : *m_LayerStack)
		{
			if (obj != nullptr)
			{
				obj->OnEvent(e);
			}
		}
	}

	void System::PushLayer(Layer* layer)
	{
		m_LayerStack->PushLayer(layer);
	}

	void System::PushOverlay(Layer* overlay)
	{
		m_LayerStack->PushLayer(overlay);
	}

	bool System::OnWindowClose(WindowCloseEvent e)
	{
		m_Window->Release();
		return true;
	}

	bool System::OnWindowResize(WindowResizeEvent e)
	{ 
		return true;
	}

	Shared<Scene> System::LoadScene(const std::string& path)
	{
		SceneSerializer serializer(nullptr);
		return serializer.Deserialize(path);
	}

	bool System::GenerateWindow()
	{
		bool result = true;
		WindowMode winMode;
		winMode.Width = Width;
		winMode.Height = Height;
		winMode.Title = Title;
		winMode.Api = WindowAPI::SFML;
		winMode.VSync = true;
		winMode.AntialiasingLevel = 16;
		m_Window = Window::Create(winMode);
		if (m_Window != nullptr)
		{
			m_Window->SetEventCallback(BIND_EVENT_FUNC(System::OnEvent));
			m_Window->Init();
		}
		else
		{
			FZLOG_WARN("윈도우 생성중 문제를 알 수 없는 오류가 발생했습니다.");
			result = false;
		}
		if (result)
			FZLOG_INFO("Window 생성 완료");
		else
			FZLOG_WARN("Window 생성 실패");
		return result;
	}

	bool System::GenerateLayerStack()
	{
		bool result = true;
		if (m_LayerStack && m_LayerStack->size() > 0)
		{
			FZLOG_WARN("Layer 스택이 이미 생성 되어 있습니다.");
			result = false;
		}
		else 
		{
			FZLOG_INFO("Layer Pool 생성");
			LayerGenerator = CreateShared<LayerPool>();
			FZLOG_INFO("Layer 스택 생성");
			m_LayerStack = CreateShared<LayerStack>(LayerGenerator);
			if (m_LayerStack == nullptr)
				result = false;
		}
		if (result)
			FZLOG_INFO("Layer 스택 생성 완료");
		else
			FZLOG_WARN("Layer 스택 생성 실패");
		return result;
	}

	bool System::ReleaseLayerStack()
	{
		bool result = true;
		FZLOG_INFO("Layer 스택 해제");
		m_LayerStack->Release();

		if (result)
			FZLOG_INFO("Layer 스택 해제 완료");
		else
			FZLOG_WARN("Layer 스택 해제 실패");
		return result;
	}

} // namespace fz
