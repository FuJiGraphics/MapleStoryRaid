#pragma once

#include "Core/Core.h"

namespace sf {
	class WindowBase;
}

namespace fz {

	enum class Axis
	{
		Horizontal,
		Vertical,
	};

	struct AxisInfo
	{
		Axis axis;
		std::list<int> positives;
		std::list<int> negatives;
		float sensi = 10.0f;
		float value = 0.0f;

		void AddKey(bool positive, int code)
		{
			if (positive)
				positives.push_back(code);
			else
				negatives.push_back(code);
		}
	};


	class Editor2D;

	class InputManager
	{
	protected:
		friend fz::Editor2D;

	public:
		static void Init();
		static void Update(float dt);
		static void PollEvent(const sf::Event& ev);

		inline static bool IsKeyDown(KeyType keycode) {
			return InputManager::IsKeyDownImpl(keycode);
		}

		inline static bool IsKeyPressed(KeyType keycode) {
			return InputManager::IsKeyPressedImpl(keycode); 
		}

		inline static bool IsKeyReleased(KeyType keycode) {
			return InputManager::IsKeyReleasedImpl(keycode);
		}

		inline static bool IsKeyCombinationPressed(const std::vector<KeyType>& keys) {
			return InputManager::IsKeyCombinationPressedImpl(keys);
		}
		
		inline static bool IsMouseDown(MouseButtonType button) {
			return InputManager::IsMouseDownImpl(button);
		}

		inline static bool IsMouseButtonPressed(MouseButtonType button) {
			return InputManager::IsMouseButtonPressedImpl(button);
		}

		inline static bool IsMouseButtonReleased(MouseButtonType button) {
			return InputManager::IsMouseButtonReleasedImpl(button);
		}

		inline static sf::Vector2f GetMousePosition() {
			return InputManager::GetMousePositionImpl();
		}

		inline static void SetTargetTrackingWindow(sf::WindowBase* window) {
			s_TargetWindow = window;
		}

		static float GetAxisRaw(Axis axis);
		static float GetAxis(Axis axis);

	protected:
		static bool IsKeyPressedImpl(KeyType keycode);
		static bool IsKeyReleasedImpl(KeyType keycode);
		static bool IsKeyDownImpl(KeyType keycode);
		static bool IsKeyCombinationPressedImpl(const std::vector<KeyType>& keys);
		static bool IsMouseDownImpl(MouseButtonType button);
		static bool IsMouseButtonPressedImpl(MouseButtonType button);
		static bool IsMouseButtonReleasedImpl(MouseButtonType button);
		static sf::Vector2f GetMousePositionImpl();
		static bool Contains(const std::list<int>& list, int code);
		static void SetEditorMode(bool enabled);
		static void SetViewportMousePos(int x, int y);
		static void SetViewportBounds(const sf::Vector2f& b1, const sf::Vector2f& b2);

	private:
		static sf::WindowBase* s_TargetWindow;
		static std::unordered_map<Axis, AxisInfo> s_AxisInfoMap;
		static std::unordered_map<sf::Keyboard::Key, bool> s_KeyStates;
		static std::unordered_map<sf::Keyboard::Key, bool> s_PrevKeyStates;
		static std::unordered_map<sf::Mouse::Button, bool> s_MouseButtonStates;
		static std::unordered_map<sf::Mouse::Button, bool> s_PrevMouseButtonStates;
		static bool s_IsEditorMode;
		static sf::Vector2i s_MousePosFromViewport;
		static sf::Vector2f s_ViewportBounds[2];
	};

	using Input = fz::InputManager;

} // namespace fz


