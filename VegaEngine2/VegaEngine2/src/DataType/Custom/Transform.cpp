#include "pch.h"
#include "Transform.h"

namespace fz {
	Transform::Transform()
		: m_Translate{ 0.0f, 0.0f }
		, m_Rotation{ 0.0f }
		, m_Scale{ 1.0f, 1.0f }
		, m_Origin{ 0.0f, 0.0f }
		, s_World{ sf::Transform::Identity }
	{
	}
	void Transform::SetTranslate(const sf::Vector2f& pos)
	{
		m_Translate = pos;
		ResetTransform();
	}

	void Transform::SetTranslate(float posX, float posY)
	{
		m_Translate = { posX, posY };
		ResetTransform();
	}

	void Transform::SetRotation(float angle)
	{
		m_Rotation = angle;
		ResetTransform();
	}

	void Transform::SetScale(const sf::Vector2f& factors)
	{
		m_Scale = factors;
		ResetTransform();
	}

	void Transform::SetScale(float factorX, float factorY)
	{
		m_Scale = { factorX, factorY };
		ResetTransform();
	}

	void Transform::SetOrigin(const sf::Vector2f& origin)
	{
		m_Origin = origin;
		ResetTransform();
	}

	void Transform::SetOrigin(float originX, float originY)
	{
		m_Origin = { originX, originY };
		ResetTransform();
	}

	sf::Vector2f Transform::GetTranslate() const
	{
		return m_Translate;
	}

	float Transform::GetRotation() const
	{
		return m_Rotation;
	}

	sf::Vector2f Transform::GetScale() const
	{
		return m_Scale;
	}

	sf::Vector2f Transform::GetOrigin() const
	{
		return m_Origin;
	}

	sf::Transform& Transform::GetRawTransform()
	{
		return s_World;
	}

	const sf::Transform& Transform::GetRawTransform() const
	{
		return s_World;
	}

	sf::Transform Transform::operator*(const fz::Transform& other) const
	{
		return s_World * other.GetRawTransform();
	}

	sf::Vector2f Transform::operator*(const sf::Vector2f& other) const
	{
		return s_World * other;
	}

	Transform& Transform::operator=(const fz::Transform& other)
	{
		this->SetTranslate(other.GetTranslate());
		this->SetRotation(other.GetRotation());
		this->SetScale(other.GetScale());
		this->SetOrigin(other.GetOrigin());
		return *this;
	}

	Transform::operator sf::Transform& ()
	{
		return s_World;
	}

	Transform::operator const sf::Transform& () const
	{
		return s_World;
	}

	void Transform::ResetTransform()
	{
		s_World = sf::Transform::Identity;
		s_World.translate(m_Translate);
		s_World.rotate(m_Rotation);
		s_World.scale(m_Scale);
		s_World.translate(m_Origin * -1.0f);
	}

} // namespace fz
