#include "pch.h"
#include "Renderer2D.h"

namespace fz {

	sf::Sprite Renderer2D::s_Spirte;
	sf::RenderWindow* Renderer2D::s_RenderWindow = nullptr; 
	Weak<Framebuffer> Renderer2D::s_FrameBuffer = nullptr;
	OrthoCamera* Renderer2D::s_OrthoCamera = nullptr;
	sf::Vector2f Renderer2D::s_PrevCameraPos = { 0.0f, 0.0f };
	std::multimap<int, RenderFrame> Renderer2D::s_CommandBuffer;

	void Renderer2D::Init(sf::RenderWindow* renderWindow)
	{
		FZLOG_ASSERT(renderWindow, "Renderer2D를 초기화할 수 없습니다.RenderWindow가 nullptr입니다.");
		s_RenderWindow = renderWindow;
	}

	void Renderer2D::Release()
	{
		s_RenderWindow = nullptr;
	}

	void Renderer2D::Reset()
	{
		s_RenderWindow = nullptr;
	}

	void Renderer2D::BeginScene(OrthoCamera& camera, Shared<Framebuffer>& framebuffer)
	{
		FZLOG_ASSERT(s_RenderWindow, "Renderer2D를 사용할 수 없습니다. 초기화되지 않은 Renderer2D 입니다.");
		FZLOG_ASSERT(framebuffer, "Renderer2D를 사용할 수 없습니다. 프레임 버퍼를 찾을 수 없습니다.");
		FZLOG_ASSERT(!s_OrthoCamera, "Renderer2D를 사용할 수 없습니다. 이미 호출된 BeginScene입니다.");

		s_FrameBuffer = framebuffer;
		s_OrthoCamera = &camera;
		s_PrevCameraPos = s_OrthoCamera->GetCenter();
		if (s_FrameBuffer)
		{
			s_FrameBuffer->Clear();
			s_FrameBuffer->GetBuffer().setView(*s_OrthoCamera);
		}
	}

	void Renderer2D::BeginScene(OrthoCamera& camera, const sf::Transform& transform, Shared<Framebuffer>& framebuffer)
	{
		FZLOG_ASSERT(s_RenderWindow, "Renderer2D를 사용할 수 없습니다. 초기화되지 않은 Renderer2D 입니다.");
		FZLOG_ASSERT(framebuffer, "Renderer2D를 사용할 수 없습니다. 프레임 버퍼를 찾을 수 없습니다.");
		FZLOG_ASSERT(!s_OrthoCamera, "Renderer2D를 사용할 수 없습니다. 이미 호출된 BeginScene입니다.");

		s_FrameBuffer = framebuffer;
		s_OrthoCamera = &camera;
		s_PrevCameraPos = s_OrthoCamera->GetCenter();
		// 일단 카메라가 오브젝트의 포지션만 따라가게 하도록 설정
		s_OrthoCamera->SetCenter(transform * s_OrthoCamera->GetCenter());
		// s_OrthoCamera->SetRotation(transform.GetRotation());
		if (s_FrameBuffer)
		{
			s_FrameBuffer->Clear();
			s_FrameBuffer->GetBuffer().setView(*s_OrthoCamera);
		}
	}

	void Renderer2D::EndScene()
	{
		FZLOG_ASSERT(s_RenderWindow, "Renderer2D를 사용할 수 없습니다. 초기화되지 않은 Renderer2D 입니다.");
		FZLOG_ASSERT(s_OrthoCamera, "Renderer2D를 사용할 수 없습니다. BeginScene이 호출되지 않았습니다.");

		if (s_FrameBuffer)
		{
			for (auto& it : s_CommandBuffer)
			{
				RenderFrame& renderFrame = it.second;
				sf::RenderStates state;
				state.transform = renderFrame.Transform * renderFrame.subTranform;
				state.texture = renderFrame.Texture;
				s_FrameBuffer->GetBuffer().draw(*renderFrame.drawable, state);
			}

			auto& renderBuffer = s_FrameBuffer->GetBuffer();
			renderBuffer.display();
			s_RenderWindow->clear();
			sf::Sprite sprite;
			sprite.setTexture(renderBuffer.getTexture());
			s_RenderWindow->draw(sprite);
		}
		// 트랜스폼 반복 적용을 위해서 카메라를 이전 위치로 복구
		if (s_OrthoCamera)
		{
			s_OrthoCamera->SetCenter(s_PrevCameraPos);
		}
		s_FrameBuffer = nullptr;
		s_OrthoCamera = nullptr;
		s_CommandBuffer.clear();
	}

	void Renderer2D::Draw(int order, sf::Drawable& drawable, const sf::Transform& transform, const sf::Transform& subTransform)
	{
		RenderFrame renderFrame = { &drawable, nullptr, transform, subTransform };
		s_CommandBuffer.insert({ order, renderFrame });
	}

	void Renderer2D::Draw(int order, sf::Sprite& sprite, const sf::Transform& transform, const sf::Transform& subTransform)
	{
		RenderFrame renderFrame = { &sprite, sprite.getTexture(), transform, subTransform };
		s_CommandBuffer.insert({ order, renderFrame });
	}

	void Renderer2D::PostDraw(sf::RectangleShape* target, const sf::Transform& transform, const sf::Transform& subTransform)
	{
		const auto& lastElement = *s_CommandBuffer.rbegin();
		RenderFrame renderFrame = { target, nullptr, transform, subTransform };
		s_CommandBuffer.insert({ lastElement.first + 1, renderFrame });
	}

} // namespace fz

 