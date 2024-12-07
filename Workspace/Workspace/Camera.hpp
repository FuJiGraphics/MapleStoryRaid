#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Map/MapComponent.h"
#include "Player/SaveData.hpp"

namespace fz {

	class CameraScript : public VegaScript
	{
	public:
		float Speed = 2000.f;
		bool IsCollideBlockL = false;
		bool IsCollideBlockR = false;
		sf::Vector2f prevPos;
		GameObject CurrentMap;
		bool isFirstUpdate = true;

		void Start() override
		{
			GameObject target = GetCurrentScene()->GetEntityFromTag("Player");
			
			const auto& targetPos = target.GetWorldPosition();
			auto& transform = GetComponent<TransformComponent>().Transform;
			transform.SetTranslate(targetPos);

			auto& body = GetComponent<RigidbodyComponent>();
			body.SetGravityScale(0.0f);

			//this->FindMap(CurrentMap);
		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			if (isFirstUpdate)
			{
				isFirstUpdate = false;
				this->FindMap(CurrentMap);
			}
			auto& body = GetComponent<RigidbodyComponent>();
			// Callback으로 바꾸기
			if (SaveData::ChangedScene)
			{
				this->FindMap(CurrentMap);
				auto& info = CurrentMap.GetComponent<MapComponent>();
				sf::Vector2f mapCentor;
				Utils::SetOrigin(mapCentor, Origins::MC, info.Size);
				body.SetPosition(mapCentor);
			}
			auto& mapInfo = CurrentMap.GetComponent<MapComponent>();

			GameObject target = GetCurrentScene()->GetEntityFromTag("Player");
			auto& transform = target.GetComponent<TransformComponent>();
			const auto& targetPos = transform.Transform.GetTranslate();
			sf::Vector2f currPos = GetWorldPosition();

			float moveSpeed = Speed * dt;
			sf::Vector2f newPos = Utils::Lerp(currPos, targetPos, moveSpeed);

			if (std::abs(targetPos.y - currPos.y) < 30.f)
			{
				newPos.y = currPos.y;
			}
			
			//auto& camera = GetComponent<CameraComponent>().Camera;
			//const sf::Vector2f& viewSize = camera.GetSize();
			//float viewHalfWidth = viewSize.x * 0.5f;
			//float mapWidth = mapInfo.Size.x;

			//float viewRightWidth = newPos.x + viewHalfWidth;
			//float mapRightWidth;mapInfo.Position.x + mapWidth;
			//if (viewRightWidth >= mapRightWidth)
			//	return;
			//float viewleftWidth = newPos.x - viewHalfWidth;
			//float mapLeftWidth = mapInfo.Position.x;
			//if (viewleftWidth <= mapLeftWidth)
			//	return;

			body.AddPositionNoGravity({ newPos.x - currPos.x, newPos.y - currPos.y });
		}

		void FindMap(GameObject& dst)
		{
			auto entityList = GetCurrentScene()->GetEntitiesFromComponent<MapComponent>();
			for (auto& entity : entityList)
			{
				dst = entity;
				break;
			}
		}
	};
} // namespace fz

