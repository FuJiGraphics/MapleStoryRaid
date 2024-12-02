#pragma once
#include "Entity.h"
#include "Scene.h"

namespace fz {

	struct Collision
	{
		GameObject gameObject;
		std::string tag = "";
		unsigned int contactCount = 0;
		sf::Vector2f impulse;
	};

	struct Collider
	{
		std::string tag = "";
	};

	class VegaScript
	{
	public:
		VegaScript() = default;
		virtual ~VegaScript() = default;

		template <typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

		template <typename T>
		bool HasComponent()
		{
			return m_Entity.HasComponent<T>();
		}

		template <typename T>
		T& AddComponent()
		{
			return m_Entity.AddComponent<T>();
		}
		
		void SetColorWithChilds(const sf::Color& color)
		{
			m_Entity.SetColorWithChilds(color);
		}

		fz::Entity& GetCurrentEntity()
		{
			return m_Entity;
		}

		sf::Vector2f GetWorldPosition()
		{
			return m_Entity.GetWorldPosition();
		}

		Weak<Scene>& GetCurrentScene()
		{
			return m_Entity.GetCurrentScene();
		}

		virtual void Start() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(float dt) {}

		virtual void OnTriggerEnter(Collider collider) {}
		virtual void OnTriggerStay(Collider collider) {}
		virtual void OnTriggerExit(Collider collider) {}

		virtual void OnCollisionEnter(Collision collision) {}
		virtual void OnCollisionStay(Collision collision) {}
		virtual void OnCollisionExit(Collision collision) {}

	private:
		Entity m_Entity;

		friend fz::Scene;
	};

} // namespace fz
