#pragma once
#include <SFML/Graphics.hpp>

namespace fz {

	class Transform
	{
	public:
		Transform();
		Transform(const Transform& other) = default;
		virtual ~Transform() = default;

		void SetTranslate(const sf::Vector2f& pos);
		void SetTranslate(float posX, float posY);
		void SetRotation(float angle);
		void SetScale(const sf::Vector2f& factors);
		void SetScale(float factorX, float factorY);
		void SetOrigin(const sf::Vector2f& origin);
		void SetOrigin(float originX, float originY);

		sf::Vector2f GetTranslate() const;
		float GetRotation() const;
		sf::Vector2f GetScale() const;
		sf::Vector2f GetOrigin() const;

		sf::Transform& GetRawTransform();
		const sf::Transform& GetRawTransform() const;

		sf::Transform operator*(const fz::Transform& other) const;
		sf::Vector2f operator*(const sf::Vector2f& other) const;
		Transform& operator=(const fz::Transform& other);
		operator sf::Transform& ();
		operator const sf::Transform& () const;

	private:
		void ResetTransform();

	private:
		sf::Vector2f	m_Translate;
		float			m_Rotation;
		sf::Vector2f	m_Scale;
		sf::Vector2f	m_Origin;
		sf::Transform	s_World;
	};

} // namespace fz