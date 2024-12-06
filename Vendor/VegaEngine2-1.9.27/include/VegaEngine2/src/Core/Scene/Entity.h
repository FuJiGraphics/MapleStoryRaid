#pragma once

#include "Core/Core.h"
#include "entt/entt.hpp"
#include "Scene.h"
#include "Components.h"

namespace fz {

	class EntitySerializer;
	class HierarchyPanel;
	struct RigidbodyComponent;

	class Entity 
	{
	protected:
		friend fz::Scene;
		friend fz::EntitySerializer;
		friend fz::SceneSerializer;
		friend fz::HierarchyPanel;

	public:
		Entity();
		Entity(entt::entity handle, const Shared<Scene>& scene);
		Entity(const std::string& uuid, entt::entity handle, const Shared<Scene>& scene);
		Entity(const Entity& other);
		Entity(Entity&& other) noexcept;

		template <typename T, typename ...Args>
		T& AddComponent(Args&&... args)
		{
			FZLOG_ASSERT(m_Handle != entt::null, "앤티티가 존재하지 않습니다.");
			bool result = !HasComponent<T>();
			FZLOG_ASSERT(result, "엔티티에 동일한 컴포넌트가 이미 존재합니다.");
			return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
		}

		template <typename T>
		void RemoveComponent()
		{
			FZLOG_ASSERT(m_Handle != entt::null, "앤티티가 존재하지 않습니다.");
			FZLOG_ASSERT(HasComponent<T>(), "엔티티에서 제거하려는 컴포넌트를 찾을 수 없습니다.");
			m_Scene->m_Registry.remove<T>(m_Handle);
		}

		template <typename T>
		T& GetComponent()
		{
			FZLOG_ASSERT(m_Handle != entt::null, "앤티티가 존재하지 않습니다.");
			FZLOG_ASSERT(HasComponent<T>(), "컴포넌트를 찾을 수 없습니다.");
			return m_Scene->m_Registry.get<T>(m_Handle);
		}

		template <typename T>
		bool HasComponent()
		{
			FZLOG_ASSERT(m_Handle != entt::null, "앤티티가 존재하지 않습니다.");
			return m_Scene->m_Registry.all_of<T>(m_Handle);
		}

		template <typename T>
		bool HasComponent() const
		{
			FZLOG_ASSERT(m_Handle != entt::null, "앤티티가 존재하지 않습니다.");
			return m_Scene->m_Registry.all_of<T>(m_Handle);
		}

		GameObject GetRootParent();
		bool GetActive();

		void SetActive(bool enabled);
		void SetColorWithChilds(const sf::Color& color);
		sf::Vector2f GetWorldPosition();
		sf::Transform GetWorldTransform();
		Weak<Scene>& GetCurrentScene() { return m_Scene; }

		inline bool operator ==(const fz::Entity& other) const	{ return m_Handle == other.m_Handle && m_Scene == other.m_Scene; }
		inline bool operator !=(const fz::Entity& other) const	{ return !((*this) == other); }
		inline operator bool() const							{ return m_Handle != entt::null; }
		inline operator std::uint32_t() const					{ return (std::uint32_t)m_Handle; }

		Entity& operator=(const Entity& other);
		Entity& operator=(Entity&& other);

	protected:
		fz::Entity CreateChildEntity(const std::string& uuid, const std::string& tagName);
		void SavePrefab(const std::string& path);
		void SetActiveWithChild(bool enabled);
		sf::Transform GetRealWorldTransform(fz::Entity parent) const;
		void DeleteRigidbodyWithChilds();

	protected:
		inline static std::uint32_t s_EntityCount = 1;

	private:
		entt::entity	m_Handle;
		Weak<Scene>		m_Scene;
		std::string		m_UUID;
	};

	using GameObject = fz::Entity;

} // namespace fz