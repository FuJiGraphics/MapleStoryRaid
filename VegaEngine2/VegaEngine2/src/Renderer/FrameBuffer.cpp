#include "pch.h"
#include "FrameBuffer.h"

namespace fz {

	Shared<Framebuffer> Framebuffer::Create(const FramebufferSpec& specification)
	{
		return CreateShared<FrameBufferSFML>(specification);
	}

} // namespace fz