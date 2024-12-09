#include "Editor2D.h"

namespace fz {
	Editor2D::Editor2D(const std::string& name)
		: Layer(name)
		, m_ActiveScene(nullptr)
		, m_SceneState(EditorState::Edit)
		, m_HierarchyPanel(m_ActiveScene, &m_SceneState)
		, m_EditorCamera({ (float)FRAMEWORK.GetWidth(), (float)FRAMEWORK.GetHeight() }, false)
		, m_ViewportBounds{ {0.f, 0.f}, {0.f, 0.f} }
		, m_Active(true)
	{
		// Empty
	}

	void Editor2D::OnAttach()
	{
		FZLOG_INFO("Editor2D 시작");
		TEXTURE_MGR.Load("graphics/player.png");
		TEXTURE_MGR.Load("editor/icons/playbutton.png");
		TEXTURE_MGR.Load("editor/icons/stopbutton.png");
		SceneManager::AddChangedSceneEvent(BIND_EVENT_FUNC(Editor2D::ChangeSceneEvent));
		bool loadedScene = false;
		if (Utils::CanFileOpen(g_TempProjectPath))
		{
			loadedScene = SceneManager::LoadScene(g_TempProjectPath);
		}
		if (!loadedScene)
		{
			SceneManager::NewScene();
		}
		InputManager::SetEditorMode(true);
	}

	void Editor2D::OnDetach()
	{
		FZLOG_INFO("Editor2D 종료");
		TEXTURE_MGR.Unload("Graphics/player.png");
		TEXTURE_MGR.Unload("editor/icons/playbutton.png");
		TEXTURE_MGR.Unload("editor/icons/stopbutton.png");
	}

	void Editor2D::OnUpdate(float dt)
	{
		switch (m_SceneState)
		{
			case EditorState::Edit:
				SceneManager::UpdateForEditor(dt, m_EditorCamera);
				SpriteEditor::OnUpdate(dt);
				break;
			case EditorState::Play:
				SceneManager::Update(dt);
				break;
		}
	}

	void Editor2D::OnEvent(fz::Event& ev)
	{
		if (m_SceneState == EditorState::Edit)
		{
			m_EditorCamera.OnEvent(ev);
			SpriteEditor::OnEvent(ev);
		}
	}

	void Editor2D::OnUI()
	{
		ImGui::BeginMainMenuBar();
		auto nativeWindow = (sf::RenderWindow*)System::GetSystem().GetWindow().GetNativeWindow();
		HWND handle = (HWND)nativeWindow->getSystemHandle();

		// 저장 핫 키
		static bool isHotkeyDown = false;
		if (!isHotkeyDown && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_S))
		{
			isHotkeyDown = true;
			if (m_SceneState == EditorState::Edit)
			{
				SceneManager::SaveScene();
				FZLOG_INFO("Scene Save UUID = {0}", m_ActiveScene->GetUUID());
			}
		}
		else
			isHotkeyDown = false;

		if (ImGui::BeginMenu("File"))
		{
			if (m_SceneState == EditorState::Edit && ImGui::MenuItem("New", "Ctrl+N"))
			{
				SceneManager::NewScene();
			}
			if (m_SceneState == EditorState::Edit && ImGui::MenuItem("Open...", "Ctrl+O"))
			{
				SceneManager::LoadScene();
			}
			if (m_SceneState == EditorState::Edit && ImGui::MenuItem("Save", "Ctrl+S"))
			{
				SceneManager::SaveScene();
			}
			if (m_SceneState == EditorState::Edit && ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
			{
				std::string path = VegaUI::SaveFile(handle, "Scene File (*.vega)\0*.vega\0");
				if (!path.empty())
				{
					SceneManager::SaveScene(path);
				}
			}
			if (m_SceneState == EditorState::Edit && ImGui::MenuItem("Exit"))
			{
				System::GetSystem().ExitSystem();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Sprite Editor"))
			{
				SpriteEditor::SetActive(true);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Game Mode"))
			{
				if (this->m_Active)
				{
					ImGuiManager::SetDocking(true);
					this->SetActive(false);
					this->m_HierarchyPanel.SetActive(false);
					// m_ActiveScene->OnViewportResize(FRAMEWORK.GetWidth(), FRAMEWORK.GetHeight());
				}
				else
				{
					ImGuiManager::SetDocking(true);
					this->SetActive(true);
					this->m_HierarchyPanel.SetActive(true);
					InputManager::SetEditorMode(true);
				}
			}
			ImGui::EndMenu();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		if (ImGui::Begin("Viewports"))
		{
			if (m_ActiveScene)
			{
				ImVec2 vpSize = ImGui::GetContentRegionAvail();
				auto& frameBuffer = m_ActiveScene->GetFrameBuffer();
				ImGui::Image(*frameBuffer);
				if (vpSize.x > 0.0f && vpSize.y > 0.0f &&
					frameBuffer->GetWidth() != (unsigned int)vpSize.x || frameBuffer->GetHeight() != (unsigned int)vpSize.y)
				{
					m_ActiveScene->OnViewportResize((unsigned int)vpSize.x, (unsigned int)vpSize.y);
				}
			}
			// 마우스 이동 허용 여부
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsWindowFocused())
				m_EditorCamera.SetActivated(true);
			else
				m_EditorCamera.SetActivated(false);

			const auto& viewportOffset = ImGui::GetCursorPos();
			const auto& windowSize = ImGui::GetWindowSize();
			const ImVec2& contentRegionMin = ImGui::GetWindowContentRegionMin();
			auto minBound = ImGui::GetWindowPos();
			minBound.x += viewportOffset.x;
			minBound.y += viewportOffset.y;

			ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
			m_ViewportBounds[0] = { minBound.x, minBound.y };
			m_ViewportBounds[1] = { maxBound.x, maxBound.y };
			InputManager::SetViewportBounds(m_ViewportBounds[0], m_ViewportBounds[1]);
			auto [mx, my] = ImGui::GetMousePos();
			mx -= m_ViewportBounds[0].x;
			my -= (minBound.y - windowSize.y);
			my -= contentRegionMin.y;
			sf::Vector2f viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
			InputManager::SetViewportMousePos((int)mx, (int)my);
		}
		// Viewport End
		ImGui::End();
		ImGui::PopStyleVar();

		this->UiToolbar();
		if (m_Active)
		{
			m_HierarchyPanel.OnImGuiRender();
			if (ImGui::Begin("Draw Debug Mode"))
			{
				if (m_ActiveScene)
				{
					bool flag = m_ActiveScene->IsDebugDisplayMode();
					if (ImGui::Checkbox("##debugMode", &flag))
					{
						m_ActiveScene->SetDebugDisplayMode(flag);
					}
				}
			}
			ImGui::End(); // Viewports
		}

		SpriteEditor::OnUI();

		ImGui::EndMainMenuBar();
	}

	void Editor2D::SetActive(bool enabled)
	{
		m_Active = enabled;
	}

	void Editor2D::ChangeSceneEvent(Shared<Scene> scene)
	{
		m_ActiveScene = scene;
		if (m_ActiveScene == nullptr)
		{
			SceneManager::NewScene(FRAMEWORK.GetWidth(), FRAMEWORK.GetHeight());
		}
		SpriteEditor::SetContext(m_ActiveScene);
		m_HierarchyPanel.SetContext(m_ActiveScene, &m_SceneState);
	}

	void Editor2D::OnScenePlay()
	{
		m_SceneState = EditorState::Play;
		SceneManager::SaveScene(g_TempProjectPath);
		SceneManager::LoadScene(g_TempProjectPath);
		SceneManager::StartScene();
	}

	void Editor2D::OnSceneStop()
	{
		m_SceneState = EditorState::Edit;
		SceneManager::StopScene();
		SceneManager::RollbackScene();
	}

	void Editor2D::UiToolbar(const char* title)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGuiWindowFlags flag = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		if (ImGui::Begin(title, nullptr, flag))
		{
			float size = ImGui::GetWindowHeight() - 4.0f;
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton("##PlayButton", TEXTURE_MGR.Get("editor/icons/PlayButton.png"), { size, size }))
			{
				if (m_SceneState == EditorState::Edit)
					OnScenePlay();
			}
			ImGui::SameLine();
			if (ImGui::ImageButton("##StopButton", TEXTURE_MGR.Get("editor/icons/StopButton.png"), { size, size }))
			{
				if (m_SceneState == EditorState::Play)
					OnSceneStop();
			}
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

} // namespace fz