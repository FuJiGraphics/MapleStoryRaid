#pragma once
#include "Entity.h"

namespace fz {

	class EntitySerializer
	{
	public:
		EntitySerializer(const fz::Entity& entity);

		void Serialize(fz::json& json);
		void Deserialize(fz::json& json);

	protected:
		void ChildSerialize(fz::json& json, fz::Entity& child);
		void ChildDerialize(fz::json& json, fz::Entity& child);

		void SerializeTag(json& json);
		void SerializeCamera(json& json);
		void SerializeTransform(json& json);
		void SerializeSprite(json& json);
		void SerializeText(json& json);
		void SerializeRigidBody(json& json);
		void SerializeCollider(json& json);

		void DeserializeTag(json& json);
		void DeserializeCamera(json& json);
		void DeserializeTransform(json& json);
		void DeserializeSprite(json& json);
		void DeserializeText(json& json);
		void DeserializeRigidBody(json& json);
		void DeserializeCollider(json& json);

		template<typename T>
		T& FindComponent()
		{
			if (!m_Entity.HasComponent<T>())
				return m_Entity.AddComponent<T>();
			else
				return m_Entity.GetComponent<T>();
		}

	private:
		fz::Entity m_Entity;
		std::string m_SceneUUID;
		std::string m_EntityUUID;
		std::string m_Path;
	};

} // namespace fz