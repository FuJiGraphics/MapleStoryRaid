#pragma once
#include <VegaEngine2.h>

namespace fz {

	class UIScript : public VegaScript
	{
	public:
		TransformComponent* transform;
		GameObject target;

		void Start() override
		{
			transform = &GetComponent<TransformComponent>();
			target = GetCurrentScene()->GetEntityFromTag("Camera");
			const sf::Vector2f& pos = target.GetComponent<TransformComponent>().Transform.GetTranslate();
			transform->Transform.SetTranslate(pos);
			const auto& viewport = GetCurrentScene()->GetViewportSize();
		}

		void OnDestroy() override
		{

		}

		void OnUpdate(float dt) override
		{
			const auto& viewport = GetCurrentScene()->GetViewportSize();
			const sf::Vector2f& pos = target.GetComponent<TransformComponent>().Transform.GetTranslate();
			float zoom = target.GetComponent<CameraComponent>().Camera.GetZoom();
			float dw = (viewport.x * zoom) / 800.f;
			float dh = (viewport.y * zoom) * 0.125f / 71.f;
			transform->Transform.SetTranslate({ pos.x, (pos.y + ((viewport.y * 0.125f) * 3.5f) * zoom) });
			transform->Transform.SetScale(dw, dh);
		}

		void OnTriggerEnter(Collider collider) override
		{

		}

		void OnTriggerStay(Collider collider) override
		{

		}

		void OnTriggerExit(Collider collider) override
		{

		}
	};
} // namespace fz

