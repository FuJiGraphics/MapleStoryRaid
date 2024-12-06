#include "pch.h"
#include "Raycast.h"

namespace fz {

	RayCastCallback::RayCastCallback()
		: m_Info()
		, m_HitDetected(false)
	{
		// Empty
	}

	float RayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
	{
		m_Info.Fixture = fixture;
		m_Info.Point = point;
		m_Info.Normal = normal;
		m_Info.Fraction = fraction;
		m_HitDetected = true;
		return m_Info.Fraction;
	}

	RayCastInfo RayCastCallback::GetInfo() const
	{
		return m_Info;
	}

} // namespace fz