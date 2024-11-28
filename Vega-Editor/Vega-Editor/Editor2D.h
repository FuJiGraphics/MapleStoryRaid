#pragma once
#include <VegaEngine2.h>
#include "Panels/HierarchyPanel.h"
#include "Editors/SpriteEditor.h"

namespace fz {

	const std::string g_TempProjectPath = "temp.json";

	enum class EditorState {
		Edit = 0, Play, Count
	};

	class Editor2D : public fz::Layer
	{
	protected:
		FZ_DELETE_COPY(Editor2D)

	public:
		Editor2D(const std::string& name);

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float dt) override;
		void OnEvent(fz::Event& ev) override;
		void OnUI() override;

	protected:
		Shared<Scene> CreateScene(unsigned int width, unsigned int height);
		void SaveScene(const Shared<Scene>& scene, const std::string& path);
		Shared<Scene> LoadScene(const std::string& path);

		void OnScenePlay();
		void OnSceneStop();
		void UiToolbar(const char* title = "##Toolbar");

		void BindScript();

	private:
		std::string	m_ActiveSceneFilePath;
		Shared<Scene> m_TempScene;
		Shared<Scene> m_ActiveScene;
		HierarchyPanel m_HierarchyPanel;
		EditorState m_SceneState;
		EditorCamera m_EditorCamera;
	};

} // namespace fz