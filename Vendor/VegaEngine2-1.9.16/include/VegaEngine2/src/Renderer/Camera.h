#pragma once

#include "Core/Core.h"

namespace fz {

	class OrthoCamera
	{
	protected:
		// Using
		using Super = fz::OrthoCamera;

	public:
        OrthoCamera();
        OrthoCamera(const OrthoCamera& other);
        OrthoCamera(OrthoCamera&& other);
        OrthoCamera(const sf::FloatRect& viewport);
        OrthoCamera(const sf::Vector2f& center, const sf::Vector2f& size);
        OrthoCamera(const sf::View& view);
		virtual ~OrthoCamera() = default;

        void SetCenter(float x, float y);
        void SetCenter(const sf::Vector2f& center);
        void SetSize(float width, float height);
        void SetSize(const sf::Vector2f& size);
        void SetRotation(float angle);
        void SetViewport(const sf::FloatRect& viewport);
        void Reset(const sf::FloatRect& rectangle);

        float GetZoom() const;
        const sf::Vector2f& GetCenter() const;
        const sf::Vector2f& GetSize() const;
        float GetRotation() const;
        const sf::FloatRect& GetViewport() const;
        void Run(float offsetX, float offsetY);
        void Run(const sf::Vector2f& offset);
        void Rotate(float angle);
        void Zoom(float factor);
        const sf::Transform& GetTransform() const;
        const sf::Transform& GetInverseTransform() const;

        const sf::View& GetView() const;
        sf::View& GetView();

        operator sf::View& () { return m_View; }
        operator const sf::View& () const { return m_View; }

        OrthoCamera& operator=(const OrthoCamera& other);

	private:
		sf::View	    m_View;
        sf::Vector2f    m_OldSize;
        sf::Vector2f    m_Size;
        float           m_Zoom;
	};


} // namespace fz