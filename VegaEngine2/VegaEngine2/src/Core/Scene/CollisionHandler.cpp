#include "pch.h"
#include "CollisionHandler.h"
#include "VegaScript.h"


namespace fz {
	void CollisionHandler::BeginContact(b2Contact* contact)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		auto handleA = static_cast<entt::entity>(fixtureA->GetUserData().pointer);
		auto handleB = static_cast<entt::entity>(fixtureB->GetUserData().pointer);

		fz::Entity entityA = { handleA, m_Context->shared_from_this() };
		fz::Entity entityB = { handleB, m_Context->shared_from_this() };

		std::string tagA = entityA.GetComponent<TagComponent>().Tag;
		std::string tagB = entityB.GetComponent<TagComponent>().Tag;

		if (entityA.HasComponent<NativeScriptComponent>())
		{
			auto& scriptComp = entityA.GetComponent<NativeScriptComponent>();
			if (fixtureA->IsSensor())
			{
				scriptComp.Instance->OnTriggerEnter({ tagB });
			}
			else
			{
				scriptComp.Instance->OnCollisionEnter({ entityB, tagB, 0, {0.0f, 0.0f} });
			}
		}
		if (entityB.HasComponent<NativeScriptComponent>())
		{
			auto& scriptComp = entityB.GetComponent<NativeScriptComponent>();
			if (fixtureB->IsSensor())
			{
				scriptComp.Instance->OnTriggerEnter({ tagA });
			}
			else
			{
				scriptComp.Instance->OnCollisionEnter({ entityA, tagA, 0, {0.0f, 0.0f} });
			}
		}
	}

	void CollisionHandler::EndContact(b2Contact* contact)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		auto handleA = static_cast<entt::entity>(fixtureA->GetUserData().pointer);
		auto handleB = static_cast<entt::entity>(fixtureB->GetUserData().pointer);

		fz::Entity entityA = { handleA, m_Context->shared_from_this() };
		fz::Entity entityB = { handleB, m_Context->shared_from_this() };

		std::string tagA = entityA.GetComponent<TagComponent>().Tag;
		std::string tagB = entityB.GetComponent<TagComponent>().Tag;

		if (entityA.HasComponent<NativeScriptComponent>())
		{
			auto& scriptComp = entityA.GetComponent<NativeScriptComponent>();
			if (fixtureA->IsSensor())
			{
				scriptComp.Instance->OnTriggerEnter({ tagB });
			}
			else
			{
				scriptComp.Instance->OnCollisionEnter({ entityB, tagB, 0 });
			}
		}
		if (entityB.HasComponent<NativeScriptComponent>())
		{
			auto& scriptComp = entityB.GetComponent<NativeScriptComponent>();
			if (fixtureB->IsSensor())
			{
				scriptComp.Instance->OnTriggerExit({ tagA });
			}
			else
			{
				scriptComp.Instance->OnCollisionExit({ entityA, tagA, 0 });
			}
		}
	}

	void CollisionHandler::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		auto handleA = static_cast<entt::entity>(fixtureA->GetUserData().pointer);
		auto handleB = static_cast<entt::entity>(fixtureB->GetUserData().pointer);

		fz::Entity entityA = { handleA, m_Context->shared_from_this() };
		fz::Entity entityB = { handleB, m_Context->shared_from_this() };

		std::string tagA = entityA.GetComponent<TagComponent>().Tag;
		std::string tagB = entityB.GetComponent<TagComponent>().Tag;

		unsigned int count = static_cast<unsigned int>(impulse->count);
		b2Vec2 totalImpulse = { 0.0f, 0.0f };
		b2WorldManifold worldMainfold;
		contact->GetWorldManifold(&worldMainfold);
		for (int i = 0; i < b2_maxManifoldPoints; ++i)
		{
			b2Vec2 normal = worldMainfold.normal;
			b2Vec2 normalImpulse = impulse->normalImpulses[i] * normal;
			b2Vec2 tangent = b2Cross(normal, 1.0f);
			b2Vec2 tangentImpulse = impulse->tangentImpulses[i] * tangent;
			b2Vec2 pointImpulse = normalImpulse + tangentImpulse;
			totalImpulse += pointImpulse;
		}

		if (entityA.HasComponent<NativeScriptComponent>())
		{
			auto& scriptComp = entityA.GetComponent<NativeScriptComponent>();
			if (fixtureA->IsSensor())
			{
				scriptComp.Instance->OnTriggerStay({ tagB });
			}
			else
			{
				scriptComp.Instance->OnCollisionStay({ entityB, tagB, count, Utils::MeterToPixel(totalImpulse) });
			}
		}
		if (entityB.HasComponent<NativeScriptComponent>())
		{
			auto& scriptComp = entityB.GetComponent<NativeScriptComponent>();
			if (fixtureB->IsSensor())
			{
				scriptComp.Instance->OnTriggerStay({ tagA });
			}
			else
			{
				scriptComp.Instance->OnCollisionStay({ entityA, tagA, count, Utils::MeterToPixel(totalImpulse) });
			}
		}
	}

	void CollisionHandler::SetContext(const Shared<Scene>& scene)
	{
		m_Context = scene;
	}

} // namespace fz
