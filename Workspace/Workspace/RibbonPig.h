#pragma once
#include <VegaEngine2.h>

namespace fz {

	class RibbonPig : public VegaScript
	{
	public:
		bool isJumping = false; // 점프 상태 추적

		Animator animator;
		AnimationClip idle;

		AnimationClip idle1;

		AnimationClip move;
		AnimationClip hit;
		AnimationClip die;



		void Start() override
		{
			auto& transform = GetComponent<TransformComponent>();
			auto& body = GetComponent<RigidbodyComponent>();
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			body.SetGravityScale(1.5f);
			animator.SetTarget(sprite, transform);
			animator.SetSpeed(1.0f);
			bool flag = idle.loadFromFile("json/animation/ribbon_pig_anim.json");
			bool flag2 = move.loadFromFile("json/animation/ribbon_pig_animMove.json");
			//bool flag3 = idle.loadFromFile("json/ribbon_pig_animHit.json");
			//bool flag4 = idle.loadFromFile("json/ribbon_pig_animDie.json");

		}

		void OnDestroy() override
		{
			FZLOG_DEBUG("리본돼지 스크립트 파괴! {0} {1}", 1.1, "aSDASCA");
		}

		void OnUpdate(float dt) override
		{
			if (!HasComponent<RigidbodyComponent>())
				return;

			animator.Update(dt);

			animator.Play(&idle);

			//	animator.Play(&move);

			fz::Transform& transform = GetComponent<TransformComponent>();
			auto& body = GetComponent<RigidbodyComponent>();

			if (Input::IsKeyDown(KeyType::D))
			{
				body.AddPosition({ 100.0, 0.0f });
				transform.SetScale(-1.0f, 1.0f);
					animator.Play(&move);
			}
			else if (Input::IsKeyDown(KeyType::A))
			{
				body.AddPosition({ -100.0, 0.0f });
				animator.Play(&move);

			}

			// 점프 처리
			if (body.IsOnGround() && !isJumping)
			{
				if (Input::IsKeyDown(KeyType::Space))
				{
					body.AddPosition({ 0.0f, -500.f });
					isJumping = true; // 점프 시작
				}
			}

			// 바닥에 닿으면 점프 상태 해제
			if (body.IsOnGround())
			{
				isJumping = false;
			}

		}

	private:

	};

}

BIND_SCRIPT(RibbonPig, "8fc401b3-19ba-4183-833f-2cf6b0c13d01", "RibbonPig", RibbonPig);




