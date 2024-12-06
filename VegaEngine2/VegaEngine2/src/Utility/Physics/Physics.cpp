#include "pch.h"
#include "Physics.h"

namespace fz {

	Physics_internal& Physics_internal::GetInstance()
	{
		static Physics_internal _instance;
		return _instance;
	}

	void Physics_internal::Raycast(const sf::Vector2f& origin, float angle, RaycastHit& hitInfo, float maxDistance)
	{
		sf::Vector2f dir = Utils::GetRotateVector(angle, { 1.0f, 0.0f });
		Physics_internal::Raycast(origin, dir, hitInfo, maxDistance);
	}

	void Physics_internal::Raycast(
		const sf::Vector2f& origin,
		const sf::Vector2f& direction,
		RaycastHit& hitInfo, float
		maxDistance)
	{
		Scene& scene = SceneManager::GetCurrentScene();
		b2World* world = scene.s_World;

		FZLOG_ASSERT(world, "Physics world is not initialized.");

		sf::Vector2f normalDir = Utils::GetNormal(direction);
		b2Vec2 rayStart = Utils::PixelToMeter(origin);
		b2Vec2 rayEnd = rayStart + Utils::PixelToMeter(maxDistance * normalDir);

		// RayCastCallback 积己
		RayCastCallback callback;

		// Box2D Raycast 角青
		world->RayCast(&callback, rayStart, rayEnd);

		if (callback.IsHitDetected())
		{
			const RayCastInfo& info = callback.GetInfo();
			auto handle = static_cast<entt::entity>(info.Fixture->GetUserData().pointer);
			fz::Entity entity = { handle, scene.shared_from_this() };
			hitInfo.Collider.tag = entity.GetComponent<TagComponent>().Tag;
			hitInfo.Point = Utils::MeterToPixel(info.Point);
			hitInfo.Normal = { info.Normal.x ,info.Normal.y };
			hitInfo.Distance = maxDistance * info.Fraction;
			hitInfo.Transform = entity.GetComponent<TransformComponent>().Transform;
			if (entity.HasComponent<RigidbodyComponent>())
				hitInfo.Rigidbody = &entity.GetComponent<RigidbodyComponent>();
			else
				hitInfo.Rigidbody = nullptr;
		}
		else
		{
			hitInfo.Collider = Collider();
			hitInfo.Point = sf::Vector2f(0, 0);
			hitInfo.Normal = sf::Vector2f(0, 0);
			hitInfo.Distance = 0.0f;
			hitInfo.Transform = fz::Transform();
			hitInfo.Rigidbody = nullptr;
		}
	}

	void Physics_internal::SetRaycastDebug(bool enabled)
	{
		FZLOG_ASSERT(false, "固备泅");
	}

} // namespace fz