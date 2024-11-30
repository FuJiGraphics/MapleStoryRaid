#pragma once
#include <VegaEngine2.h>

namespace fz {

	class SpriteEditor
	{
	public:
		enum class SelectMode {
			Movement, Select,
		};

		static void SetContext(Shared<Scene>& scene);
		static bool SetTarget(const std::string& texturePath);
		static void SetActive(bool enabled);

		static void OnUI();
		static void OnEvent(fz::Event& ev);
		static void OnUpdate(float dt);

		static void Clear();

	protected:
		static void RenderSprite(const sf::Sprite& sprite, Shared<fz::Framebuffer>& buffer);
		static bool RenderWithSelectMode(SelectMode& dst);
		static void UpdateWithSelectMode();
		static void DrawRect(bool enabled);
		static void ClampCameraBounds();
		static void ClampCameraZoom();
		static void UpdateFrames();
		static void RenderFrameViewport();
		static void SaveAnimationClip(const std::string& path);
		static void OpenAnimationClip(const std::string& path);

	private:
		inline static bool s_IsOpenedFile = false;
		inline static bool s_IsActive = false;
		inline static SelectMode s_Mode = SelectMode::Movement;
		inline static sf::Vector2f start = { 0.0f, 0.0f };
		inline static sf::Vector2f end = { 0.0f, 0.0f };
		inline static sf::Sprite s_WorkSprite;
		inline static Weak<Scene> s_Scene;
		inline static Shared<Framebuffer> s_FrameBuffer;
		inline static Shared<Framebuffer> s_PreviewBuffer;
		inline static std::vector<sf::Sprite> s_Frames;
		static EditorCamera s_EditorCamera;
	};

} // namespace fz