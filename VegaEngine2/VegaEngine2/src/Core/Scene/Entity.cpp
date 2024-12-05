#include "pch.h"
#include "Entity.h"
#include "EntitySerializer.h"

namespace fz {
	Entity::Entity()
		: m_Handle{ entt::null }
		, m_Scene(nullptr)
		, m_UUID(Random.GetUUID())
	{
		// Empty
	}

	Entity::Entity(entt::entity handle, const Shared<Scene>& scene)
		: m_Handle(handle)
		, m_Scene(scene)
		, m_UUID(Random.GetUUID())
	{
		// Empty
	}

	Entity::Entity(const std::string& uuid, entt::entity handle, const Shared<Scene>& scene)
		: m_Handle(handle)
		, m_Scene(scene)
		, m_UUID(uuid)
	{
		// Empty
	}

	Entity::Entity(const Entity& other)
		: m_Handle(other.m_Handle)
		, m_Scene(other.m_Scene)
		, m_UUID(other.m_UUID)
	{
		// Empty
	}

	Entity::Entity(Entity&& other) noexcept
		: m_Handle(other.m_Handle)
		, m_Scene(other.m_Scene)
		, m_UUID(other.m_UUID)
	{
		other.m_Handle = entt::null;
		other.m_Scene = nullptr;
		other.m_UUID = "";
	}

	GameObject Entity::GetRootParent()
	{
		if (!HasComponent<RootEntityComponent>())
		{
			auto& parent = GetComponent<ParentEntityComponent>().ParentEntity;
			return parent.GetRootParent();
		}
		return { m_Handle, m_Scene->shared_from_this() };
	}

	bool Entity::GetActive()
	{
		return this->GetComponent<TagComponent>().Active;
	}

	void Entity::SetActive(bool enabled)
	{
		auto& tagComp = this->GetComponent<TagComponent>();
		tagComp.Active = enabled;
		if (HasComponent<ChildEntityComponent>())
		{
			auto& childComp = GetComponent<ChildEntityComponent>();
			for (auto& child : childComp.CurrentChildEntities)
			{
				child.SetActive(enabled);
			}
		}
	}

	void Entity::SetColorWithChilds(const sf::Color& color)
	{
		if (this->HasComponent<SpriteComponent>())
		{
			sf::Sprite& sprite = this->GetComponent<SpriteComponent>();
			sprite.setColor(color);
		}
		if (this->HasComponent<ChildEntityComponent>())
		{
			auto& childComp = this->GetComponent<ChildEntityComponent>();
			for (auto& child : childComp.CurrentChildEntities)
			{
				child.SetColorWithChilds(color);
			}
		}
	}

	sf::Vector2f Entity::GetWorldPosition()
	{
		if (this->HasComponent<RootEntityComponent>())
		{
			auto& transformComp = this->GetComponent<TransformComponent>();
			return transformComp.Transform.GetTranslate();
		}

		const auto& parent = GetComponent<ParentEntityComponent>().ParentEntity;
		const sf::Transform realWorldTransform = GetRealWorldTransform(parent);
		const auto& localPos = GetComponent<TransformComponent>().Transform.GetTranslate();

		return realWorldTransform * localPos;
	}

	sf::Transform Entity::GetWorldTransform()
	{
		if (this->HasComponent<RootEntityComponent>())
		{
			return GetComponent<TransformComponent>().Transform;
		}

		const auto& parent = GetComponent<ParentEntityComponent>().ParentEntity;
		const sf::Transform realWorldTransform = GetRealWorldTransform(parent);
		const sf::Transform& localTransform = GetComponent<TransformComponent>().Transform;

		return realWorldTransform * localTransform;
	}

	Entity& Entity::operator=(const Entity& other)
	{
		this->m_Handle = other.m_Handle;
		this->m_Scene = other.m_Scene;
		this->m_UUID = other.m_UUID;
		return *this;
	}

	Entity& Entity::operator=(Entity&& other)
	{
		this->m_Handle = other.m_Handle;
		this->m_Scene = other.m_Scene;
		this->m_UUID = other.m_UUID;
		other.m_Handle = entt::null;
		other.m_Scene = nullptr;
		other.m_UUID = "";
		return *this;
	}

	fz::Entity Entity::CreateChildEntity(const std::string& uuid, const std::string& tagName)
	{
		fz::Entity childEntity = {};
		const auto& it = m_Scene->m_EntityPool.find(uuid);
		if (it == m_Scene->m_EntityPool.end())
		{
			childEntity = m_Scene->CreateEntityWithUUID(tagName, uuid);
			if (!childEntity.HasComponent<ParentEntityComponent>())
			{
				auto& parentComp = childEntity.AddComponent<ParentEntityComponent>();
				parentComp.ParentEntity = *this;
			}

			if (this->HasComponent<ChildEntityComponent>())
			{
				auto& childComp = this->GetComponent<ChildEntityComponent>();
				childComp.ParentEntity = *this;
				childComp.CurrentChildEntities.push_back(childEntity);
			}
			else
			{
				auto& childComp = this->AddComponent<ChildEntityComponent>();
				childComp.ParentEntity = *this;
				childComp.CurrentChildEntities.push_back(childEntity);
			}
			auto& dhildTransformComp = childEntity.GetComponent<TransformComponent>();
			dhildTransformComp.IsChildRenderMode = true;
		}
		return childEntity;
	}

	void Entity::SavePrefab(const std::string& path)
	{
		Database::LoadFromJson(path);
		auto& json = Database::GetJsonObject(path);
		json.clear();
		EntitySerializer serializer(*this);
		serializer.Serialize(json);
		Database::Unload(path);
	}

	void Entity::SetActiveWithChild(bool enabled)
	{
		if (this->HasComponent<TagComponent>())
		{
			auto& tagComp = this->GetComponent<TagComponent>();
			tagComp.Active = enabled;
		}
		if (this->HasComponent<ChildEntityComponent>())
		{
			auto& childComp = this->GetComponent<ChildEntityComponent>();
			for (auto& child : childComp.CurrentChildEntities)
			{
				child.SetActiveWithChild(enabled);
			}
		}
	}

	sf::Transform Entity::GetRealWorldTransform(fz::Entity parent) const
	{
		if (parent.HasComponent<RootEntityComponent>())
		{
			return parent.GetComponent<TransformComponent>().Transform;
		}
		sf::Transform transform = parent.GetComponent<TransformComponent>().Transform;
		auto& parentComp = parent.GetComponent<ParentEntityComponent>();
		return GetRealWorldTransform(parentComp.ParentEntity) * transform;
	}

	void Entity::DeleteRigidbodyWithChilds()
	{
		if (HasComponent<ChildEntityComponent>())
		{
			auto& comp = GetComponent<ChildEntityComponent>();
			for (auto& child : comp.CurrentChildEntities)
			{
				child.DeleteRigidbodyWithChilds();
			}
		}
		if (HasComponent<RigidbodyComponent>())
		{
			auto& rigidComp = GetComponent<RigidbodyComponent>();
			b2World* currWorld = GetCurrentScene()->s_World;
			if (rigidComp.RuntimeBody) 
			{
				if (currWorld && !currWorld->IsLocked())
				{
					currWorld->DestroyBody((b2Body*)rigidComp.RuntimeBody);
					rigidComp.RuntimeBody = nullptr;
				}
			}
		}
	}


} // namespace fz