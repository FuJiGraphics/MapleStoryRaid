#pragma once

namespace fz {
	
	class Framebuffer;
	class OrthoCamera;
	class Window;

	struct RenderFrame
	{
		sf::Sprite*	Sprite = nullptr;
		const sf::Transform* Transform = nullptr;
		sf::RectangleShape* RectangleShape = nullptr;
		sf::CircleShape* CircleShape = nullptr;
		sf::Transform subTranform = sf::Transform::Identity;
	};

	class Renderer2D
	{
	public:
		static void Init(sf::RenderWindow* renderWindow);
		static void Release();
		static void Reset();

		static void BeginScene(OrthoCamera& camera, Shared<Framebuffer>& framebuffer);
		static void BeginScene(OrthoCamera& camera, const sf::Transform& transform, Shared<Framebuffer>& framebuffer);
		static void EndScene();

		static void Draw(int order, sf::Sprite& target, const sf::Transform& transform, const sf::Transform& subTransform = sf::Transform::Identity);
		static void Draw(sf::RectangleShape* target, const sf::Transform& transform, const sf::Transform& subTransform = sf::Transform::Identity);
	private:
		static sf::Sprite s_Spirte;
		static sf::RenderWindow* s_RenderWindow;
		static Weak<Framebuffer> s_FrameBuffer;
		static OrthoCamera* s_OrthoCamera;
		static sf::Vector2f s_PrevCameraPos;
		static std::multimap<int, RenderFrame> s_CommandBuffer;
	};

} // namespace fz

