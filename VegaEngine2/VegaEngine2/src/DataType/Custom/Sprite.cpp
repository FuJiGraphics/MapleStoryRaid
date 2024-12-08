#include "pch.h"
#include "Sprite.h"

namespace fz {
	Sprite::Sprite(const fz::Sprite& other)
		: m_TexPath("")
		, m_Origins(Origins::MC)
	{
		m_Sprite = other.m_Sprite;
		m_TexPath = other.m_TexPath;
	}

	Sprite::Sprite(const std::string& texPath)
		: m_TexPath(texPath)
		, m_Origins(Origins::MC)
	{
		const sf::Texture& texture = TEXTURE_MGR.Get(texPath);
		m_Sprite.setTexture(texture);
		m_Sprite.setTextureRect({ 0, 0, (int)texture.getSize().x, (int)texture.getSize().y });
		m_Size = texture.getSize();
		m_TexPath = texPath;
	}

	void Sprite::SetTextureWithMask(const std::string& texPath, const sf::Color& alphaMask)
	{
		if (Utils::CanFileOpen(texPath) == false)
			return;

		m_IsMaskMode = true;
		sf::Image image;
		image.loadFromFile(texPath);
		image.createMaskFromColor(alphaMask);
		sf::Texture& texture = TEXTURE_MGR.Get(texPath);
		texture.create(image.getSize().x, image.getSize().y);
		texture.loadFromImage(image);
		m_Sprite.setTexture(texture);
		m_Sprite.setTextureRect({ 0, 0, (int)texture.getSize().x, (int)texture.getSize().y });
		m_TexPath = texPath;
		m_Size = texture.getSize();
		m_Mask = alphaMask;
	}

	void Sprite::SetTexture(const sf::Texture& texture)
	{
		m_Sprite.setTexture(texture);
		m_Sprite.setTextureRect({ 0, 0, (int)texture.getSize().x, (int)texture.getSize().y });
		m_Size = texture.getSize();
		m_TexPath = "Copy Texture";
	}

	void Sprite::SetTexture(const std::string& texPath)
	{
		const sf::Texture& texture = TEXTURE_MGR.Get(texPath);
		m_Sprite.setTexture(texture);
		m_Sprite.setTextureRect({ 0, 0, (int)texture.getSize().x, (int)texture.getSize().y });
		m_Size = texture.getSize();
		m_TexPath = texPath;
		SetOrigins(m_Origins);
	}

	void Sprite::SetColor(const sf::Color& color)
	{
		m_Sprite.setColor(color);
	}

	void Sprite::SetOrigins(Origins origins)
	{
		m_Origins = origins;
		if (origins != Origins::Custom)
			Utils::SetOrigin(m_Sprite, origins);
	}

	void Sprite::SetMaskColor(bool enabled, const sf::Color& alphaMask)
	{
		m_IsMaskMode = enabled;
		m_Mask = alphaMask;
		if (m_IsMaskMode)
		{
			sf::IntRect prevRect = m_Sprite.getTextureRect();
			this->SetTextureWithMask(m_TexPath, m_Mask);
			this->SetTextureRect(prevRect);
		}
		else
		{
			sf::IntRect prevRect = m_Sprite.getTextureRect();
			this->SetTexture(m_TexPath);
			this->SetTextureRect(prevRect);
		}
	}

	void Sprite::SetTextureRect(const sf::IntRect& rect)
	{
		m_Sprite.setTextureRect(rect);
	}

} // namespace fz