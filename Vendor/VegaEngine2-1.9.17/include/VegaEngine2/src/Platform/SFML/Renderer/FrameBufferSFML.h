#pragma once

#include "Renderer/FrameBuffer.h"

namespace fz {

	class FrameBufferSFML : public Framebuffer
	{
	public:
		FrameBufferSFML(const FramebufferSpec& specification);

		FramebufferSpec GetInfo() const override;
		sf::RenderTexture& GetBuffer() override;
		void Resize(unsigned int width, unsigned int height) override;
		void Clear() override;

		operator sf::RenderTexture& () override;
		operator const sf::RenderTexture& () const override;

		inline unsigned int GetWidth() const override	{ return m_Spec.Width; }
		inline unsigned int GetHeight() const override	{ return m_Spec.Height; }

	private:
		FramebufferSpec m_Spec;
		sf::RenderTexture m_Buffer;
	};

} // namespace fz