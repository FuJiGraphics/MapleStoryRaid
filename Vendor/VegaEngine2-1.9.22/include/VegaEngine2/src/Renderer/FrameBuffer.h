#pragma once

namespace fz {

	struct FramebufferSpec
	{
		int	Width;
		int	Height;
		int	MultisampleLevel = 1;
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		static Shared<Framebuffer> Create(const FramebufferSpec& specification);

		virtual FramebufferSpec GetInfo() const = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual sf::RenderTexture& GetBuffer() = 0;
		virtual void Resize(unsigned int width, unsigned int height) = 0;
		virtual void Clear() = 0;

		virtual operator sf::RenderTexture& () = 0;
		virtual operator const sf::RenderTexture& () const = 0;
	};

} // namespace fz
