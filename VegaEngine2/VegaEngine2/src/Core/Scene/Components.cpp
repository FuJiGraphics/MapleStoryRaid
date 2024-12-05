#include "pch.h"

namespace fz {

	void RigidbodyComponent::AddForce(const sf::Vector2f& force)
	{
		if (!RuntimeBody)
			return;
		((b2Body*)RuntimeBody)->ApplyForceToCenter(Utils::PixelToMeter(force), true);
	}

	void RigidbodyComponent::SetGravityScale(float scale)
	{
		if (!RuntimeBody)
			return;
		((b2Body*)RuntimeBody)->SetGravityScale(scale);
	}

	void RigidbodyComponent::SetLinearVelocity(const sf::Vector2f& velocity)
	{
		if (!RuntimeBody)
			return;
		((b2Body*)RuntimeBody)->SetLinearVelocity(Utils::PixelToMeter(velocity));
	}

	void RigidbodyComponent::AddPosition(const sf::Vector2f& pos)
	{
		sf::Vector2f velocity = this->GetLinearVelocity();
		if (std::isinf(velocity.x) || std::isinf(velocity.y))
			velocity = { 0.0f, 0.0f };
		if (std::isnan(velocity.x) || std::isnan(velocity.y))
			velocity = { 0.0f, 0.0f };
		float nextPosX = Utils::IsEqual(pos.x, 0.0f) ? velocity.x : pos.x;
		float nextPosY = Utils::IsEqual(pos.y, 0.0f) ? velocity.y : pos.y;
		this->SetLinearVelocity({ nextPosX, nextPosY });
	}

	void RigidbodyComponent::AddPositionNoGravity(const sf::Vector2f& pos)
	{
		sf::Vector2f velocity = this->GetLinearVelocity();
		if (std::isinf(velocity.x) || std::isinf(velocity.y))
			velocity = { 0.0f, 0.0f };
		if (std::isnan(velocity.x) || std::isnan(velocity.y))
			velocity = { 0.0f, 0.0f };
		float nextPosX = Utils::IsEqual(pos.x, 0.0f) ? 0.0f : pos.x;
		float nextPosY = Utils::IsEqual(pos.y, 0.0f) ? 0.0f : pos.y;
		this->SetLinearVelocity({ nextPosX, nextPosY });
	}

	void RigidbodyComponent::SetPosition(const sf::Vector2f& pos)
	{
		if (!RuntimeBody)
			return;
		b2Body* body = static_cast<b2Body*>(RuntimeBody);
		if (body) {
			body->SetTransform(Utils::PixelToMeter(pos), 0.0f);
		}
	}

	void RigidbodyComponent::SetPosition(const sf::Vector2f& pos, float angle)
	{
		if (!RuntimeBody)
			return;
		b2Body* body = static_cast<b2Body*>(RuntimeBody);
		if (body) {
			body->SetTransform(Utils::PixelToMeter(pos), angle);
		}
	}

	sf::Vector2f RigidbodyComponent::GetLinearVelocity() const
	{
		if (RuntimeBody == nullptr)
			return { 0.0f, 0.0f };
		return Utils::MeterToPixel(((b2Body*)RuntimeBody)->GetLinearVelocity());
	}

	bool RigidbodyComponent::IsOnGround(float rayLen)
	{
		sf::Vector2f empty;
		float fempty;
		return IsOnGround(rayLen, empty, empty, fempty);
	}

	bool RigidbodyComponent::IsOnGround(float rayLen, sf::Vector2f& normal)
	{
		sf::Vector2f empty;
		float fempty;
		return IsOnGround(rayLen, normal, empty, fempty);
	}

	bool RigidbodyComponent::IsOnGround(float rayLen, sf::Vector2f& normal, sf::Vector2f& pos)
	{
		float fempty;
		return IsOnGround(rayLen, normal, pos, fempty);
	}

	bool RigidbodyComponent::IsOnGround(float rayLen, sf::Vector2f& normal, sf::Vector2f& pos, float& fraction)
	{
		if (Scene::s_World == nullptr)
			return false;
		if (RuntimeBody == nullptr)
			return false;

		class RayCastCallback : public b2RayCastCallback
		{
		public:
			RayCastCallback() = default;
			~RayCastCallback() = default;

			float Fraction = 0.0f;
			bool HitGround = false;
			b2Vec2 Normal = { 0.0f, 0.0f };
			b2Vec2 Position = { 0.0f, 0.0f };
			float ReportFixture(b2Fixture* f, const b2Vec2& p, const b2Vec2& n, float fraction) override
			{
				if (!f->IsSensor() && f->GetBody()->GetType() == b2_staticBody)
				{
  					HitGround = true;
   					Normal = { n.x, n.y };
					Position = p;
					Fraction = fraction;
					return 0.0f;
				}
				return 1.0f;  // 계속 진행
			}
		};
		RayCastCallback callback;

		b2Vec2 start = ((b2Body*)RuntimeBody)->GetPosition();
		b2Vec2 end = start + b2Vec2(0.0f, 5.0f);
		Scene::s_World->RayCast(&callback, start, end);
		b2Vec2 norm = { callback.Normal.x * -1.0f, callback.Normal.y * -1.0f };
		callback.HitGround = false;
		
		end = start + b2Vec2(norm.x * rayLen, norm.y * rayLen);
		bool hitGround = false; 
		if (!Utils::IsEqual(end.y, start.y) || !Utils::IsEqual(end.x, start.x))
		{
			Scene::s_World->RayCast(&callback, start, end);
			normal = { callback.Normal.x, callback.Normal.y };
			pos = Utils::MeterToPixel(callback.Position);
			hitGround = callback.HitGround;
		}
 		return hitGround;
	}

	void BoxCollider2DComponent::SetTrigger(bool enabled)
	{
		IsTrigger = enabled;
		((b2Fixture*)RuntimeFixture)->SetSensor(IsTrigger);
	}

	void EdgeCollider2DComponent::SetTrigger(bool enabled)
	{
		IsTrigger = enabled;
		((b2Fixture*)RuntimeFixture)->SetSensor(IsTrigger);
	}

} // namespace fz
