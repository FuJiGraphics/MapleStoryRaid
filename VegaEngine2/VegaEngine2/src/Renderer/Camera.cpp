#include "pch.h"
#include "Camera.h"

namespace fz {

	OrthoCamera::OrthoCamera()
		: m_View()
		, m_Zoom(1.0f)
		, m_Size(1.0f, 1.0f)
		, m_OldSize(1.0f, 1.0f)
	{
		// Empty
	}

	OrthoCamera::OrthoCamera(const OrthoCamera& other)
		: m_View(other.m_View)
		, m_Size(other.m_Size)
		, m_Zoom(other.m_Zoom)
		, m_OldSize(other.m_OldSize)
	{
		// Empty
	}

	OrthoCamera::OrthoCamera(OrthoCamera&& other)
		: m_View(other.m_View)
		, m_Size(other.m_Size)
		, m_Zoom(other.m_Zoom)
		, m_OldSize(other.m_OldSize)
	{
		other.m_View = sf::View();
		other.m_Size = { 1.0f, 1.0f };
		other.m_Zoom = 1.0f;
		other.m_OldSize = { 1.0f, 1.0f };
	}

	OrthoCamera::OrthoCamera(const sf::FloatRect& viewport)
		: m_View(viewport)
		, m_Zoom(1.0f)
		, m_Size(1.0f, 1.0f)
		, m_OldSize(1.0f, 1.0f)
	{
		// Empty
	}

	OrthoCamera::OrthoCamera(const sf::Vector2f& center, const sf::Vector2f& size)
		: m_View(center, size)
		, m_Size(size)
		, m_Zoom(1.0f)
		, m_OldSize(1.0f, 1.0f)
	{
		// Empty
	}

	OrthoCamera::OrthoCamera(const sf::View& view)
		: m_View(view)
		, m_Zoom(1.0f)
		, m_Size(1.0f, 1.0f)
		, m_OldSize(1.0f, 1.0f)
	{
		// Empty
	}

	void OrthoCamera::SetCenter(float x, float y)
	{
		m_View.setCenter(x, y);
	}

	void OrthoCamera::SetCenter(const sf::Vector2f& center)
	{
		m_View.setCenter(center);
	}

	void OrthoCamera::SetSize(float width, float height)
	{
		m_Size = { width, height };
		m_View.setSize(m_Size * m_Zoom);
	}

	void OrthoCamera::SetSize(const sf::Vector2f& size)
	{
		m_Size = size;
		m_View.setSize(m_Size * m_Zoom);
	}

	void OrthoCamera::SetRotation(float angle)
	{
		m_View.setRotation(angle);
	}

	void OrthoCamera::SetViewport(const sf::FloatRect& viewport)
	{
		m_View.setViewport(viewport);
	}

	void OrthoCamera::Reset(const sf::FloatRect& rectangle)
	{
		m_View.reset(rectangle);
	}

	float OrthoCamera::GetZoom() const
	{
		return m_Zoom;
	}

	const sf::Vector2f& OrthoCamera::GetCenter() const
	{
		return m_View.getCenter();
	}

	const sf::Vector2f& OrthoCamera::GetSize() const
	{
		return m_View.getSize();
	}

	float OrthoCamera::GetRotation() const
	{
		return m_View.getRotation();
	}

	const sf::FloatRect& OrthoCamera::GetViewport() const
	{
		return m_View.getViewport();
	}

	void OrthoCamera::Run(float offsetX, float offsetY)
	{
		m_View.move(offsetX, offsetY);
	}

	void OrthoCamera::Run(const sf::Vector2f& offset)
	{
		m_View.move(offset);
	}

	void OrthoCamera::Rotate(float angle)
	{
		m_View.rotate(angle);
	}

	void OrthoCamera::Zoom(float factor)
	{
		m_Zoom = factor;
		m_View.setSize(m_Size * m_Zoom);
	}

	const sf::Transform& OrthoCamera::GetTransform() const
	{
		return m_View.getTransform();
	}

	const sf::Transform& OrthoCamera::GetInverseTransform() const
	{
		return m_View.getInverseTransform();
	}

	const sf::View& OrthoCamera::GetView() const
	{
		return m_View;
	}

	sf::View& OrthoCamera::GetView()
	{
		return m_View;
	}

	OrthoCamera& OrthoCamera::operator=(const OrthoCamera& other)
	{
		m_View = other.m_View;
		m_Size = other.m_Size;
		m_OldSize = other.m_OldSize;
		m_Zoom = other.m_Zoom;
		return *this;
	}

} // namespace fz
