#include "pch.h"
#include "SceneManager.h"

namespace fz {
	Shared<Scene> SceneManager::NewScene(unsigned int width, unsigned int height)
	{
		if (s_HasStartScene)
		{
			SceneManager::StopScene();
		}
		Shared<Scene> newScene = CreateShared<Scene>(width, height);
		s_ActiveScene = newScene;
		s_PrevScenePath = s_ActiveScenePath;
		s_ActiveScenePath.clear();
		SceneManager::StartScene();
		SceneManager::StopScene();
		for (auto& fn : s_EventCallbacks)
		{
			fn(s_ActiveScene);
		}
		return newScene;
	}

	bool SceneManager::LoadScene(const std::string& path)
	{
		std::string loadPath = path;
		if (loadPath.empty())
		{
			auto nativeWindow = (sf::RenderWindow*)System::GetSystem().GetWindow().GetNativeWindow();
			HWND handle = (HWND)nativeWindow->getSystemHandle();
			loadPath = VegaUI::OpenFile(handle, "Scene File (*.vega)\0*.vega\0");
		}
		if (loadPath.empty())
			return false;

		if (s_HasStartScene)
		{
			SceneManager::StopScene();
		}
		SceneSerializer serializer(nullptr);
		Shared<Scene> loadScene = serializer.Deserialize(loadPath);
		s_ActiveScene = loadScene;
		s_PrevScenePath = s_ActiveScenePath;
		s_ActiveScenePath = loadPath;
		for (auto& fn : s_EventCallbacks)
		{
			fn(s_ActiveScene);
		}
		return true;
	}

	void SceneManager::SaveScene(const std::string& path)
	{
		std::string savePath = path.empty() ? s_ActiveScenePath : path;
		if (savePath.empty())
		{
			auto nativeWindow = (sf::RenderWindow*)System::GetSystem().GetWindow().GetNativeWindow();
			HWND handle = (HWND)nativeWindow->getSystemHandle();
			savePath = VegaUI::SaveFile(handle, "Scene File (*.vega)\0*.vega\0");
		}
		if (!savePath.empty() && s_ActiveScene)
		{
			if (s_HasStartScene)
			{
				SceneManager::StopScene();
			}
			SceneSerializer serializer(s_ActiveScene);
			serializer.Serialize(savePath);
		}
	}

	void SceneManager::RollbackScene()
	{
		if (s_PrevScenePath.empty())
			return;
		if (s_HasStartScene)
		{
			SceneManager::StopScene();
		}
		SceneSerializer serializer(nullptr);
		Shared<Scene> loadScene = serializer.Deserialize(s_PrevScenePath);
		s_ActiveScene = loadScene;
		s_ActiveScenePath = s_PrevScenePath;
		for (auto& fn : s_EventCallbacks)
		{
			fn(s_ActiveScene);
		}
	}

	void SceneManager::RuntimeChangeScene(const std::string& path)
	{
		if (s_HasStartScene)
		{
			s_ActiveScene->m_SceneChanged = true;
			SceneManager::StopScene();
		}
		SceneManager::LoadScene(path);
		SceneManager::StartScene();
	}

	void SceneManager::StartScene()
	{
		s_HasStartScene = true;
		AttachScripts(s_ActiveScene);
		s_ActiveScene->OnPreUpdate();
	}

	void SceneManager::StopScene()
	{
		s_HasStartScene = false;
		s_ActiveScene->OnPostUpdate();
	}

	void SceneManager::Update(float dt)
	{
		s_ActiveScene->OnUpdate(dt);
	}

	void SceneManager::UpdateForEditor(float dt, EditorCamera& editorCamera)
	{
		s_ActiveScene->OnUpdateEditor(dt, editorCamera);
	}

	void SceneManager::AddChangedSceneEvent(const CallbackFn& func)
	{
		s_EventCallbacks.push_back(func);
	}

	Scene& SceneManager::GetCurrentScene()
	{
		return *s_ActiveScene;
	}

} // namespace fz
