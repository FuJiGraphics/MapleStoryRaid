#pragma once

namespace fz {

	class Scene;

	class SceneSerializer
	{
	protected:
		FZ_DELETE_COPY(SceneSerializer);

	public:
		SceneSerializer();
		SceneSerializer(const Shared<Scene>& scene);
		~SceneSerializer() = default;

		void SetTargetScene(const Shared<Scene>& scene);

		void Serialize(const std::string& path);
		void SerializeRuntime(const std::string& path);

		Shared<Scene> Deserialize(const std::string& path);
		bool DeserializeRuntime(const std::string& path);

	private:
		Shared<Scene> m_Scene;
	};

} // namespace 