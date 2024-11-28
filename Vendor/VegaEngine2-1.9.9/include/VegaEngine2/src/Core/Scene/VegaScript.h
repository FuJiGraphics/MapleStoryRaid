#pragma once
#include "Entity.h"
#include "Scene.h"

namespace fz {

	class VegaScript
	{
	public:
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

		virtual void Start() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(float dt) {}

		virtual void OnTriggerEnter() {}
		virtual void OnTriggerStay() {}
		virtual void OnTriggerExit() {}

		virtual void OnCollisionEnter() {}
		virtual void OnCollisionStay() {}
		virtual void OnCollisionExit() {}

	private:
		Entity m_Entity;

		friend fz::Scene;
	};

} // namespace fz
