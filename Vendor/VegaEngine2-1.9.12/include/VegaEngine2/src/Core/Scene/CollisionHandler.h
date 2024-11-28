#pragma once

namespace fz {

	class CollisionHandler : public b2ContactListener
	{
	public:
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;

		// void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
		void SetContext(const Shared<Scene>& scene);

	private:
		Weak<Scene> m_Context;
	};

} // namespace fz