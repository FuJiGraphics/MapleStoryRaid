#pragma once
#include "Renderer/FrameBuffer.h"
#include "SceneSerializer.h"
#include "CollisionHandler.h"

namespace fz {
	
	// Forwards
	class Entity;
	class Framebuffer;
	class HierarchyPanel;
	class SceneSerializer;
	class EntitySerializer;
	class EditorCamera;
	class Editor2D;
	class SceneManager;

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
		friend fz::SceneManager;
		FZ_DELETE_COPY(Scene)

	public:
		Scene(unsigned int width, unsigned int height, unsigned int mulltisampleLevel = 1, const std::string& uuid = "");
		~Scene();

		void DestroyInstance(GameObject& prefabInstance);

		Entity GetEntityFromUUID(const std::string& uuid);
		Entity GetEntityFromTag(const std::string& tag);
		inline sf::Vector2u GetViewportSize() const { return { m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight() }; }
		void* GetPhysicsWorld() const { return s_World; }

		bool IsDebugDisplayMode() const { return m_IsDebugMode; }
		void SetDebugDisplayMode(bool enabled) { m_IsDebugMode = enabled; }

		sf::Vector2f GetWorldMousePos() const;

		sf::Vector2f PixelToCoords(const sf::Vector2i& pixelPos) const;
		sf::Vector2f PixelToCoords(const sf::Vector2i& pixelPos, const fz::OrthoCamera& camera) const;

		GameObject Instantiate(const std::string& tag, const sf::Vector2f& position);
		GameObject Instantiate(const std::string& tag, const sf::Vector2f& position, const sf::Vector2f& scale);
		GameObject Instantiate(GameObject entity, const sf::Vector2f& position);
		GameObject Instantiate(GameObject entity, const sf::Vector2f& position, float rotation);
		GameObject Instantiate(GameObject entity, const fz::Transform& transform);
		GameObject Instantiate(GameObject entity, const std::string& tag, const fz::Transform& transform);
		
		template <typename Component>
		std::vector<Entity> GetEntitiesFromComponent()
		{
			std::vector<Entity> entities;
			auto view = m_Registry.view<Component>();
			for (auto& entity : view)
			{
				entities.push_back({ entity, shared_from_this() });
			}
			return entities;
		}

		template <typename Component>
		void GetEntitiesFromComponent(std::vector<Entity>& dst)
		{
			auto view = m_Registry.view<Component>();
			for (auto& entity : view)
			{
				dst.push_back({ entity, shared_from_this() });
			}
		}

		template <typename... Components>
		auto GetEntities()
		{
			return m_Registry.view<Components...>();
		}

		template <typename... Components>
		auto GetEntities() const
		{
			return m_Registry.view<Components...>();
		}

	protected:
		Entity CreateEntity(const std::string& tagName = "");
		void DeleteEntity(fz::Entity& entity);

		Entity CreateEntityWithUUID(const std::string& tagName, const std::string& uuid);
		void CopyEntityForPrefab(fz::Entity dst, fz::Entity src, bool isRootTransform = false);

		inline Shared<Framebuffer>& GetFrameBuffer() { return m_FrameBuffer; }

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

		void OnCreateRuntimeInstance();
		void OnDestroyRuntimeInstance();
		void OnUpdatePhysicsSystem(float dt);
		void OnUpdateCamera(OrthoCamera** dstCamera, sf::Transform& dstTransform);
		void OnRenderEditorSprite(OrthoCamera* mainCamera);
		void OnRuntimeRenderDrawable(OrthoCamera* mainCamera, sf::Transform& transform);
		void OnDrawDebugShape();
		void OnViewportResize(unsigned int width, unsigned int height);

		void ReleasePrefabInstancies();
		void ReleasePostRemoveInstancies();

		inline const Shared<Framebuffer>& GetFrameBuffer() const { return m_FrameBuffer; }

		std::string GetUUID() const { return m_UUID; }
	public:
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
		CollisionHandler		m_CollistionHandler;
		std::list<fz::Entity>	m_RemoveInstanceList;
		std::list<fz::Entity>	m_PostRemoveInstanceList;
		std::list<fz::Entity>	m_LoadPrefabInstanceList;
		bool					m_SceneChanged;
	};

} // namespace fz