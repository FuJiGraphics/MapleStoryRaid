#pragma once

namespace fz {

	class Sprite
	{
	public:
		Sprite() = default;
		Sprite(const fz::Sprite& other);
		Sprite(const std::string& texPath);

		void SetTextureWithMask(const std::string& texPath, const sf::Color& alphaMask);
		void SetTexture(const sf::Texture& texture);
		void SetTexture(const std::string& texPath);
		void SetColor(const sf::Color& color);
		void SetOrigins(Origins origins);
		void SetMaskColor(bool enabled, const sf::Color& alphaMask);
		void SetTextureRect(const sf::IntRect& rect);

		bool IsMaskMode() const { return m_IsMaskMode; }
		sf::Color GetMaskColor() const { return m_Mask; }
		sf::Color GetColor() const { return m_Sprite.getColor(); }
		Origins GetOrigins() const { return m_Origins; }
		sf::Vector2u GetSize() const { return m_Size; }
		std::string GetTexturePath() const { return m_TexPath; }
		sf::IntRect GetTextureRect() const { return m_Sprite.getTextureRect(); }

		operator sf::Sprite& () { return m_Sprite; }
		operator const sf::Sprite& () const { return m_Sprite; }

	private:
		sf::Sprite		m_Sprite;
		std::string		m_TexPath;
		Origins			m_Origins;
		sf::Color		m_Mask;
		sf::Vector2u	m_Size;
		sf::IntRect		m_TextureRectTL;
		bool			m_IsMaskMode = false;
	};

} // namespace fz