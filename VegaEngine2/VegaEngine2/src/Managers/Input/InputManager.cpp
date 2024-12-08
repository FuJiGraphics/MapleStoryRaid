#include "pch.h"
#include "InputManager.h"

namespace fz {

	sf::WindowBase* InputManager::s_TargetWindow = nullptr;
	std::unordered_map<Axis, AxisInfo> InputManager::s_AxisInfoMap;
	std::unordered_map<sf::Keyboard::Key, bool> InputManager::s_KeyStates;
	std::unordered_map<sf::Keyboard::Key, bool> InputManager::s_PrevKeyStates;
	std::unordered_map<sf::Mouse::Button, bool> InputManager::s_MouseButtonStates;
	std::unordered_map<sf::Mouse::Button, bool> InputManager::s_PrevMouseButtonStates;
	bool InputManager::s_IsEditorMode = false;
	sf::Vector2i InputManager::s_MousePosFromViewport = { 0, 0 };
	sf::Vector2f InputManager::s_ViewportBounds[2] = { { 0.0f, 0.0f }, { 0.0f, 0.0f } };

	void InputManager::Init()
	{
		AxisInfo infoH;
		infoH.axis = Axis::Horizontal;
		infoH.AddKey(true, sf::Keyboard::D);
		infoH.AddKey(false, sf::Keyboard::A);
		s_AxisInfoMap.insert({ infoH.axis, infoH });

		// Vertical
		AxisInfo infoV;
		infoV.axis = Axis::Vertical;
		infoV.AddKey(true, sf::Keyboard::S);
		infoV.AddKey(false, sf::Keyboard::W);
		s_AxisInfoMap.insert({ infoV.axis, infoV });	

		// Init Mouse
		for (int i = 0; i < sf::Mouse::Button::ButtonCount; ++i)
		{
			s_MouseButtonStates[(sf::Mouse::Button)i] = false;
			s_PrevMouseButtonStates[(sf::Mouse::Button)i] = false;
		}

		// Init Keyboard
		for (int i = 0; i < sf::Keyboard::Key::KeyCount; ++i)
		{
			s_KeyStates[(sf::Keyboard::Key)i] = false;
			s_PrevKeyStates[(sf::Keyboard::Key)i] = false;
		}

	}

	void InputManager::Update(float dt)
	{
		for (auto& pair : s_AxisInfoMap)
		{
			auto& axisInfo = pair.second;
			float raw = GetAxisRaw(axisInfo.axis);
			float dir = raw;
			if (dir == 0.f && axisInfo.value != 0.f)
			{
				dir = axisInfo.value > 0.f ? -1.f : 1.f;
			}

			axisInfo.value += dir * axisInfo.sensi * dt;
			axisInfo.value = Utils::Clamp(axisInfo.value, -1.f, 1.f);

			float stopThreshold = std::abs(dir * axisInfo.sensi * dt);
			if (raw == 0.f && std::abs(axisInfo.value) < stopThreshold)
			{
				axisInfo.value = 0.f;
			}
		}
	}

	void InputManager::PollEvent(const sf::Event& ev)
	{
		switch (ev.type)
		{
			case sf::Event::KeyPressed:
				s_KeyStates[ev.key.code] = true;
				break;
			case sf::Event::KeyReleased:
				s_KeyStates[ev.key.code] = false;
				break;
			case sf::Event::MouseButtonPressed:
				s_MouseButtonStates[ev.mouseButton.button] = true;
				break;
			case sf::Event::MouseButtonReleased:
				s_MouseButtonStates[ev.mouseButton.button] = false;
				break;
		}
	}

	void InputManager::SetEditorMode(bool enabled)
	{
		s_IsEditorMode = enabled;
	}

	void InputManager::SetViewportMousePos(int x, int y)
	{
		s_MousePosFromViewport = { x, y };
	}

	void InputManager::SetViewportBounds(const sf::Vector2f& b1, const sf::Vector2f& b2)
	{
		s_ViewportBounds[0] = b1;
		s_ViewportBounds[1] = b2;
	}

	float InputManager::GetAxisRaw(Axis axis)
	{
		auto findIt = s_AxisInfoMap.find(axis);
		if (findIt == s_AxisInfoMap.end())
			return 0.f;

		auto& axisInfo = findIt->second;
		for (int key : axisInfo.positives)
		{
			if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(key)))
				return 1.f;
		}
		for (int key : axisInfo.negatives)
		{
			if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(key)))
				return -1.f;
		}
		return 0.0f;
	}

	float InputManager::GetAxis(Axis axis)
	{
		auto findIt = s_AxisInfoMap.find(axis);
		if (findIt == s_AxisInfoMap.end())
			return 0.f;
		return findIt->second.value;
	}

	bool InputManager::IsKeyPressedImpl(KeyType keycode)
	{
		return s_KeyStates[(sf::Keyboard::Key)keycode];
	}

	bool InputManager::IsKeyReleasedImpl(KeyType keycode)
	{
		return !InputManager::IsKeyPressedImpl(keycode);
	}

	bool InputManager::IsKeyDownImpl(KeyType keycode)
	{
		auto it = s_PrevKeyStates.find((sf::Keyboard::Key)keycode);
		if (it == s_PrevKeyStates.end())
		{
			if (InputManager::IsKeyPressedImpl(keycode))
			{
				s_PrevKeyStates.insert({ (sf::Keyboard::Key)keycode, true });
				return true;
			}
			else
			{
				s_PrevKeyStates.insert({ (sf::Keyboard::Key)keycode, false });
				return false;
			}
		}
		else
		{
			if (!it->second && InputManager::IsKeyPressed(keycode))
			{
				s_PrevKeyStates[(sf::Keyboard::Key)keycode] = true;
				return true;
			}
			else if (it->second && InputManager::IsKeyReleasedImpl(keycode))
			{
				s_PrevKeyStates[(sf::Keyboard::Key)keycode] = false;
				return false;
			}
		}
		return false;
	}

	bool InputManager::IsKeyCombinationPressedImpl(const std::vector<KeyType>& keys)
	{
		for (auto key : keys)
		{
			if (!IsKeyPressedImpl(key))
				return false;
		}
		return true;
	}

	bool InputManager::IsMouseDownImpl(MouseButtonType button)
	{
		auto it = s_PrevMouseButtonStates.find((sf::Mouse::Button)button);
		if (it == s_PrevMouseButtonStates.end())
		{
			if (!sf::Mouse::isButtonPressed((sf::Mouse::Button)button))
			{
				s_PrevMouseButtonStates[(sf::Mouse::Button)button] = false;
				return true;
			}
			else
			{
				s_PrevMouseButtonStates[(sf::Mouse::Button)button] = true;
				return false;
			}
		}
		else
		{
			if (it->second && !sf::Mouse::isButtonPressed((sf::Mouse::Button)button))
			{
				s_PrevMouseButtonStates[(sf::Mouse::Button)button] = false;
				return true;
			}
			else if (!it->second && sf::Mouse::isButtonPressed((sf::Mouse::Button)button))
			{
				s_PrevMouseButtonStates[(sf::Mouse::Button)button] = true;
				return false;
			}
		}
		return false;
	}

	bool InputManager::IsMouseButtonPressedImpl(MouseButtonType button)
	{
		return s_MouseButtonStates[(sf::Mouse::Button)button];
	}

	bool InputManager::IsMouseButtonReleasedImpl(MouseButtonType button)
	{
		return !InputManager::IsMouseButtonPressedImpl(button);
	}

	sf::Vector2f InputManager::GetMousePositionImpl()
	{
		sf::Vector2i mousePos;
		// case 0 Game Engine Editor mode
		if (s_IsEditorMode)
			mousePos = s_MousePosFromViewport;
		// case 1 Target Window Mouse Position
		else if (s_TargetWindow)
			mousePos = sf::Mouse::getPosition(*s_TargetWindow);
		// case 2 Global position in the desktop
		else
			mousePos = sf::Mouse::getPosition();
		float x = static_cast<float>(mousePos.x);
		float y = static_cast<float>(mousePos.y);
		return sf::Vector2f({ x, y });
	}

	bool InputManager::Contains(const std::list<int>& list, int code)
	{
		return std::find(list.begin(), list.end(), code) != list.end();
	}

} // namespace fz