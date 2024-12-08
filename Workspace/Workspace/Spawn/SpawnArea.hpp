#pragma once
#include <VegaEngine2.h>
#include <random>
#include <SFML/Graphics.hpp>

#pragma region Internal
namespace _internal {
	static float RandomRange(float min, float max)
	{
		static std::mt19937 generator;
		static bool isInit = false;
		if (!isInit)
		{
			isInit = true;
			std::random_device rd;
			generator.seed(rd());
		}
		std::uniform_real_distribution<float> dist(min, max);
		return dist(generator);
	}
	struct TransformComponent
	{
		sf::Transform Origin;
		sf::Transform Translate;
		sf::Transform Scale;
		sf::Transform Rotation;

		void Clear()
		{
			Origin = sf::Transform::Identity;
			Translate = sf::Transform::Identity;
			Scale = sf::Transform::Identity;
			Rotation = sf::Transform::Identity;
		}
		sf::Transform GetWorld() const
		{
			sf::Transform world = Translate * Rotation * Scale * Origin;
			return world;
		}
	};

	class Tile
	{
	public:
		Tile()
			: m_Vertices(4, sf::Vector2f(0.0f, 0.0f))
		{
			// Empty
		}
		virtual ~Tile() = default;

		sf::FloatRect GetGlobalBounds() const
		{
			return {
			m_Vertices[0].position.x, m_Vertices[0].position.y,
			m_Vertices[1].position.x, m_Vertices[2].position.y
			};
		}
		void SetSize(unsigned int width, unsigned int height)
		{
			m_Size.x = static_cast<float>(width);
			m_Size.y = static_cast<float>(height);
			m_Vertices[1].position = { m_Pos.x + width, m_Pos.y };
			m_Vertices[2].position = { m_Pos.x + width, m_Pos.y + height };
			m_Vertices[3].position = { m_Pos.x, m_Pos.y + height };
		}
		void SetSize(float width, float height)
		{
			m_Size.x = width;
			m_Size.y = height;
			m_Vertices[1].position = { m_Pos.x + width, m_Pos.y };
			m_Vertices[2].position = { m_Pos.x + width, m_Pos.y + height };
			m_Vertices[3].position = { m_Pos.x, m_Pos.y + height };
		}
		void SetPosition(float x, float y)
		{
			m_Pos = { x, y };
			m_Vertices[0].position = { m_Pos.x, m_Pos.y };
			m_Vertices[1].position = { m_Pos.x + m_Size.x, m_Pos.y };
			m_Vertices[2].position = { m_Pos.x + m_Size.x, m_Pos.y + m_Size.y };
			m_Vertices[3].position = { m_Pos.x, m_Pos.y + m_Size.y };
		}
		void SetTexCoord(float x, float y, float width, float height)
		{
			m_Vertices[0].texCoords = { x, y };
			m_Vertices[1].texCoords = { x + width, y };
			m_Vertices[2].texCoords = { x + width, y + height };
			m_Vertices[3].texCoords = { x, y + height };
		}
		void Clear()
		{
			for (int i = 0; i < 4; ++i)
			{
				m_Vertices[i].position = { 0.0f, 0.0f };
				m_Vertices[i].color = { 0, 0, 0, 0 };
				m_Vertices[i].texCoords = { 0.0f, 0.0f };
			}
			m_Pos = { 0.0f, 0.0f };
			m_Size = { 0.0f, 0.0f };
		}

		inline sf::Vector2f GetSize() const { return m_Size; }
		sf::Vertex& operator[] (int index) { return m_Vertices[index]; }
		const sf::Vertex& operator[] (int index) const { return m_Vertices[index]; }
		friend sf::VertexArray& operator<<(sf::VertexArray& vao, Tile& tile)
		{
			vao.append(tile.m_Vertices[0]);
			vao.append(tile.m_Vertices[1]);
			vao.append(tile.m_Vertices[2]);
			vao.append(tile.m_Vertices[3]);
			return vao;
		}

	private:
		std::vector<sf::Vertex> m_Vertices;
		sf::Vector2f m_Pos;
		sf::Vector2f m_Size;
	};
}
#pragma endregion

class SpawnArea
{
protected:
	SpawnArea(SpawnArea&) = delete;
	SpawnArea& operator=(const SpawnArea&) = delete;

public:
	SpawnArea()
		: outlineColor(sf::Color::Red)
		, thickness(10.0f)
		, isDrawable(true)
	{
		// Empty
	}
	virtual ~SpawnArea() = default;
	
	void SetPosition(float x, float y)
	{
		transform.Translate.translate(x, y);
	}
	void SetPosition(const sf::Vector2f& pos)
	{
		transform.Translate.translate(pos);
	}
	void SetScale(float scaleX, float scaleY)
	{
		transform.Scale.scale(scaleX, scaleY);
	}
	void SetScale(const sf::Vector2f& scale)
	{
		transform.Scale.scale(scale);
	}
	void SetSize(float sizeX, float sizeY)
	{
		this->size = { sizeX, sizeY };
		area.SetSize(sizeX, sizeY);
	}
	void SetSize(const sf::Vector2f& size)
	{
		this->size.x = size.x;
		this->size.y = size.y;
		area.SetSize(size.x, size.y);
	}
	void SetOrigin(fz::Origins preset)
	{
		sf::Vector2f newOrigin(area.GetSize().x, area.GetSize().y);
		newOrigin.x *= ((int)preset % 3) * 0.5f;
		newOrigin.y *= ((int)preset / 3) * 0.5f;
		transform.Origin.translate(newOrigin * -1.0f);
	}
	void SetOrigin(const sf::Vector2f& newOrigin)
	{
		transform.Origin.translate(newOrigin * -1.0f);
	}
	void SetOutlineColor(sf::Color color)
	{
		outlineColor = color;
	}
	void SetOutlineThickness(float thickness)
	{
		this->thickness = thickness;
	}
	void SetDrawable(bool enabled)
	{
		this->isDrawable = enabled;
	}
	void Release()
	{
		transform.Clear();
		area.Clear();
		size = { 0.0f, 0.0f };
	}
	void Draw(sf::RenderWindow& window)
	{
		if (!isDrawable)
			return;

		const auto& world = transform.GetWorld();
		const sf::Vector2f& start = world * area[0].position;
		sf::RectangleShape drawRect;
		drawRect.setFillColor(sf::Color::Transparent);
		drawRect.setOutlineColor(outlineColor);
		drawRect.setOutlineThickness(thickness);
		drawRect.setPosition({ start.x, start.y });
		drawRect.setSize({ size.x, size.y });
		window.draw(drawRect);
	}

public:
	sf::Vector2f Spawn() const
	{
		const auto& world = transform.GetWorld();
		sf::Vector2f start = world * area[0].position;
		sf::Vector2f end = world * area[2].position;
		float ranX = _internal::RandomRange(start.x, end.x);
		float ranY = _internal::RandomRange(start.y, end.y);
		return { ranX, ranY };
	}

private:
	_internal::Tile area;
	_internal::TransformComponent transform;
	sf::Vector2f size;
	sf::Color outlineColor;
	float thickness;
	bool isDrawable;
};
