#pragma once
#include <vector>
#include <functional>

namespace fz {

	class SceneManager
	{
	protected:
		using CallbackFn = std::function<void(Shared<Scene>)>;
		using EventCallbacks = std::vector<CallbackFn>;

	public:
		static Shared<Scene> NewScene(unsigned int width = 1920, unsigned int height = 1080);
		static bool LoadScene(const std::string& path = "");
		static void SaveScene(const std::string& path = "");
		static void RollbackScene();

		static void RuntimeChangeScene(const std::string& path);

		static void StartScene();
		static void StopScene();
		static void Update(float dt);
		static void UpdateForEditor(float dt, EditorCamera& editorCamera);

		static void AddChangedSceneEvent(const CallbackFn& func);
		static Scene& GetCurrentScene();

	private:
		inline static std::string s_PrevScenePath;
		inline static std::string s_ActiveScenePath;
		inline static Shared<Scene> s_ActiveScene;
		inline static EventCallbacks s_EventCallbacks;
		inline static bool s_HasStartScene = false;
	};


} // namespace fz
