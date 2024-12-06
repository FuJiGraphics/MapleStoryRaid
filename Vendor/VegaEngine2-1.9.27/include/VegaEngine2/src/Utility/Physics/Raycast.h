#pragma once
#include "Core/Scene/Components.h"
#include "Core/Scene/VegaScript.h"

namespace fz {

	struct RaycastHit
	{
		Collider Collider;							// 충돌한 객체의 Collider
		sf::Vector2f Point;							// 충돌한 월드 좌표
		sf::Vector2f Normal;						// 충돌한 표면의 노멀 벡터
		float Distance;								// 충돌 지점까지의 거리
		fz::Transform Transform;					// 충돌한 대상의 트랜스폼
		RigidbodyComponent* Rigidbody = nullptr;	// 충돌 대상의 리지드바디 컴포넌트 (없을 경우 nullptr)
	};

	struct RayCastInfo
	{
		b2Fixture* Fixture = nullptr;
		b2Vec2 Point = { 0.f, 0.f };
		b2Vec2 Normal = { 0.f, 0.f };
		float Fraction = 0.f;
	};

	class RayCastCallback : public b2RayCastCallback
	{
	public:
		RayCastCallback();
		virtual ~RayCastCallback() = default;

		float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
		RayCastInfo GetInfo() const;

		bool IsHitDetected() const { return m_HitDetected; }

	private:
		RayCastInfo m_Info;
		bool m_HitDetected;
	};

} // namespace fz
