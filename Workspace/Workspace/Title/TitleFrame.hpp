#pragma once
#include <VegaEngine2.h>
#include "FrameComponent.h"
#include "Player/PlayerStatus.hpp"

namespace fz {

	class TitleFrameScript : public VegaScript
	{
	public:
		FrameComponent* frame;
		bool isFirstUpdate = true;
		float CameraSpeed = 1.5f;

		void Start() override
		{
			frame = &AddComponent<FrameComponent>();
			frame->status = FrameStatus::Login;

		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			if (isFirstUpdate)
			{
				isFirstUpdate = false;
				auto& playerStatus = GetCurrentScene()->GetEntityFromTag("Player").GetComponent<PlayerStatusComponent>();
				playerStatus.IsLoginMode = true;
			}

			switch(frame->status)
			{
				case FrameStatus::Login:
					this->MoveLogin(dt);
					break;
				case FrameStatus::LoginDone:
					frame->status = FrameStatus::CharSelect;
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
			SceneManager::RuntimeChangeScene("game/scene/Stage1_town.vega");
		}

	};
} // namespace fz

