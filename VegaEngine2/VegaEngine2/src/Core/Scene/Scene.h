#pragma once
#include "Renderer/FrameBuffer.h"
#include "SceneSerializer.h"

class b2WorldId;

namespace fz {
	
	// Forwards
	class Entity;
	class Framebuffer;
	class HierarchyPanel;
	class SceneSerializer;
	class EntitySerializer;
	class EditorCamera;
	class Editor2D;

	using GameObject = fz::Entity;

	class Scene : public std::enable_shared_from_this<Scene>
	{
	protected:
		using Super = fz::Scene;
		using EntityPool = std::unordered_map<std::string, entt::entity>;
		friend fz::Entity;
		friend fz::HierarchyPanel;
		friend fz::SceneSerializer;
		friend fz::EntitySerializer;
		friend fz::Editor2D;
		FZ_DELETE_COPY(Scene)

	public:
		Scene(unsigned int width, unsigned int height, unsigned int mulltisampleLevel = 1, const std::string& uuid = "");
		~Scene();

		Entity CreateEntity(const std::string& tagName = "");
		void DeleteEntity(fz::Entity& entity);

		Entity GetEntityFromUUID(const std::string& uuid);
		Entity GetEntityFromTag(const std::string& tag);
		inline sf::Vector2u GetViewportSize() const { return { m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight() }; }
		inline Shared<Framebuffer>& GetFrameBuffer() { return m_FrameBuffer; }
		void* GetPhysicsWorld() const { return s_World; }

		bool IsDebugDisplayMode() const { return m_IsDebugMode; }
		void SetDebugDisplayMode(bool enabled) { m_IsDebugMode = enabled; }

		GameObject Instantiate(GameObject entity, const sf::Vector2f& position, float rotation);
		GameObject Instantiate(GameObject entity, const sf::Vector2f& position);
		GameObject Instantiate(GameObject entity, const fz::Transform& transform);
		GameObject Instantiate(GameObject entity, const std::string& tag, const fz::Transform& transform);

		template <typename... Components>
		auto GetEntities()
		{
			return m_Registry.view<Components...>();
		}

	protected:
		Entity CreateEntityWithUUID(const std::string& tagName, const std::string& uuid);
		void CopyEntityForPrefab(fz::Entity dst, fz::Entity src, bool isRootTransform = false);

		void ReleaseNativeComponent();

		void OnUpdateEditor(float dt, EditorCamera& editorCamera);
		void OnPreUpdate();
		void OnUpdate(float dt);
		void OnPostUpdate();

		// Physics
		void StartPhysics();
		void StopPhysics();
		void LoginPhysicsWorld(fz::Entity entity);

		// Scripts
		void OnPreUpdateScript();
		void OnUpdateScript(float dt);
		void OnPostUpdateScript();

		// Prefab
		void LoadPrefab(const std::string& path);

		void OnUpdateChildEntity();
		void OnUpdatePhysicsSystem(float dt);
		void OnUpdateCamera(OrthoCamera** dstCamera, sf::Transform& dstTransform);
		void OnRenderEditorSprite(OrthoCamera* mainCamera);
		void OnRenderRuntimeSprite(OrthoCamera* mainCamera, sf::Transform& transform);
		void OnDrawDebugShape();
		void OnViewportResize(unsigned int width, unsigned int height);

		void ReleasePrefabInstancies();

		inline const Shared<Framebuffer>& GetFrameBuffer() const { return m_FrameBuffer; }

		std::string GetUUID() const { return m_UUID; }
	public:
		// TODO: юс╫ц
		inline static Shared<Scene>	s_CurrentScene;
		inline static b2World*	s_World = nullptr;

	private:
		entt::registry			m_Registry;
		Shared<Framebuffer>		m_FrameBuffer;
		std::string				m_UUID;
		EntityPool				m_EntityPool;
		bool					m_IsDebugMode;
		std::string				m_prefabTempPath;
		int						m_prefabInstanceCount;
		EntityPool				m_PrefabInstancePool;
	};

#define FZ_CURRENT_SCENE fz::Scene::s_CurrentScene

} // namespace fz