#pragma once
#include <VegaEngine2.h>
#include "FadeComponent.h"

namespace fz {

	class FadeScript : public VegaScript
	{
	public:
		FadeComponent* fadeComp = nullptr;
		float elap = 0.f;
		float speed = 0.5f;

		void Start() override
		{
			fadeComp = &AddComponent<FadeComponent>();
			GetCurrentEntity().SetActive(true);
		}

		void OnUpdate(float dt) override
		{
			if (fadeComp->FadeIn)
			{
				this->FadeIn(dt);
			}
			else
			{
				this->FadeOut(dt);
			}
		}

		void FadeIn(float dt)
		{
			auto& spriteComp = GetComponent<SpriteComponent>();
			const auto& color = spriteComp.Sprite.GetColor();
			float alpha = Utils::Lerp((float)color.a, 0.f, (speed * 2.f) * dt);
			spriteComp.Sprite.SetColor({ color.r, color.g, color.b, (sf::Uint8)alpha });
			if (alpha <= 1.f)
				fadeComp->IsFadeInDone = true;
			else
				fadeComp->IsFadeInDone = false;
		}

		void FadeOut(float dt)
		{
			elap += (speed * dt);
			auto& spriteComp = GetComponent<SpriteComponent>();
			const auto& color = spriteComp.Sprite.GetColor();
			float alpha = Utils::Lerp((float)color.a, 255.f, elap);
			spriteComp.Sprite.SetColor({ color.r, color.g, color.b, (sf::Uint8)alpha });
			if (alpha >= 254.f)
			{
				fadeComp->IsFadeOutDone = true;
				elap = 0.f;
			}
			else
				fadeComp->IsFadeOutDone = false;
		}

	};
} // namespace fz


