#pragma once
#include <VegaEngine2.h>
#include "FrameComponent.h"
#include "Player/PlayerStatus.hpp"
#include "UI/FadeComponent.h"

namespace fz {

	class TitleFrameScript : public VegaScript
	{
	public:
		FrameComponent* Frame;
		bool IsFirstUpdate = true;
		bool IsStartGame = false;
		float CameraSpeed = 1.5f;
		GameObject Fade;
		void Start() override
		{
			Frame = &AddComponent<FrameComponent>();
			Frame->status = FrameStatus::Login;
			Fade = GetCurrentScene()->GetEntityFromTag("Fade");

			sf::Vector2u targetResolution = { 800, 600 };

			sf::Vector2u windowSize = GetCurrentScene()->GetViewportSize();
			GetComponent<CameraComponent>().Camera.SetSize(static_cast<float>(targetResolution.x), static_cast<float>(targetResolution.y));

			float viewportX = static_cast<float>(targetResolution.x) / static_cast<float>(windowSize.x);
			float viewportY = static_cast<float>(targetResolution.y) / static_cast<float>(windowSize.y);

			GetComponent<CameraComponent>().Camera.SetViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

			float zoom = std::min(viewportX, viewportY);
			GetComponent<CameraComponent>().Camera.Zoom(zoom);
		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			if (IsFirstUpdate)
			{
				IsFirstUpdate = false;
				auto& playerStatus = GetCurrentScene()->GetEntityFromTag("Player").GetComponent<PlayerStatusComponent>();
				playerStatus.IsLoginMode = true;
			}

			if (IsStartGame)
			{
				if (Fade.GetComponent<FadeComponent>().IsFadeOutDone)
				{
					SceneManager::RuntimeChangeScene("game/scene/Stage1_town.vega");
				}
				return;
			}

			switch (Frame->status)
			{
				case FrameStatus::Login:
					this->MoveLogin(dt);
					break;
				case FrameStatus::LoginDone:
					Frame->status = FrameStatus::CharSelect;
					break;
				case FrameStatus::LoginExit:
					break;
				case FrameStatus::CharSelect:
					this->MoveCharSelect(dt);
					break;
				case FrameStatus::CharSelectDone:
					this->StartGame();
					break;
				case FrameStatus::CharSelectExit:
					break;
			}
		}

		void MoveLogin(float dt)
		{
			fz::Transform& transform = GetComponent<TransformComponent>().Transform;
			const sf::Vector2f& currPos = transform.GetTranslate();
			if (currPos.y >= 906.f)
				transform.SetTranslate({ currPos.x, currPos.y });
			else
				transform.SetTranslate({ currPos.x, Utils::Lerp(currPos.y, 909.f, CameraSpeed * dt) });
		}

		void MoveCharSelect(float dt)
		{
			fz::Transform& transform = GetComponent<TransformComponent>().Transform;
			const sf::Vector2f& currPos = transform.GetTranslate();
			if (currPos.y <= -296.f)
				transform.SetTranslate({ currPos.x, currPos.y });
			else
				transform.SetTranslate({ currPos.x, Utils::Lerp(currPos.y, -300.f, CameraSpeed * dt) });
		}

		void CharSelectDone()
		{
			auto& playerStatus = GetCurrentScene()->GetEntityFromTag("Player").GetComponent<PlayerStatusComponent>();
			playerStatus.IsLoginMode = false;
		}

		void StartGame()
		{
			GameObject fade = GetCurrentScene()->GetEntityFromTag("Fade");
			fade.GetComponent<FadeComponent>().SetFadeOut(true);
			IsStartGame = true;
		}
	};

} // namespace fz

