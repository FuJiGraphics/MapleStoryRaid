 #pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"
#include <random>

namespace fz {

    class MushmomScript : public VegaScript, public MonsterFSM
    {
        using AnimPool = std::unordered_map<std::string, AnimationClip>;
    public:

        float JumpPower = -430.f;
        float MoveSpeed = 100.f;

        Animator animator;
        Animator animator2;

        AnimPool clips;
        AnimPool clips2;


        TransformComponent* transform;
        RigidbodyComponent* body;

        Timer timer;

        enum class AIState { Idle, Moving, CastingSkill} currentState = AIState::Moving;

        void Start() override
        {
			transform = &GetComponent<TransformComponent>();
			body = &GetComponent<RigidbodyComponent>();
			sf::Sprite& sprite = GetComponent<SpriteComponent>();
			animator.SetTarget(GetCurrentEntity());
			animator.SetSpeed(1.0f);
			clips["idle"].loadFromFile("game/animations/mushmom_idle.anim");
			clips["move"].loadFromFile("game/animations/mushmom_move.anim");
			clips["damaged"].loadFromFile("game/animations/mushmom_damaged.anim");
			clips["die"].loadFromFile("game/animations/mushmom_die.anim");
			clips["skill1"].loadFromFile("game/animations/mushmom_skill1.anim");

			sf::Sprite& effectSprite = GetComponent<SpriteComponent>();
			animator2.SetTarget(GetCurrentEntity());
			animator2.SetSpeed(1.0f);
			clips2["skilleffect1"].loadFromFile("game/animations/mushmom_skill1Effect.anim");


            body->SetGravityScale(1.5f);

            timer["ActionTimer"].Start(5.0f); // 이동 상태로 시작
            timer["JumpTimer"].Start(0.6f);   // 점프 딜레이
        }
        void OnDestroy() override
        {
            FZLOG_DEBUG("머쉬맘 스크립트 소멸");
        }
        void OnUpdate(float dt) override
        {
            if (!HasComponent<RigidbodyComponent>())
                return;

            animator.Update(dt);
            animator2.Update(dt);
            timer.Update(dt);

            if (currentState == AIState::CastingSkill)
            {
                // 스킬 지속 시간 확인
                if (timer["SkillDuration"].Done())
                {
                    currentState = AIState::Idle; // 스킬이 끝나면 Idle 상태로 복귀
                }
                return; // 스킬 중에는 다른 동작을 수행하지 않음
            }

            if (timer["ActionTimer"].Done() && currentState != AIState::CastingSkill)
            {
                // 상태 전환
                if (currentState == AIState::Moving)
                {

                    currentState = AIState::Idle;
                    timer["ActionTimer"].Start(3.0f); // Idle 지속 시간

                }
                else
                {
                    currentState = AIState::Moving;
                    timer["ActionTimer"].Start(5.0f); // 이동 지속 시간
                    MoveDirection = GetRandomDirection();
                    timer["JumpTimer"].Start(0.6f); // 점프 초기화
                }
            }

            if (timer["SkillCooldown"].Done())
            {
                UseSkill();
            }

            if (currentState == AIState::Moving)
            {
                Move(MoveDirection, dt);

                if (timer["JumpTimer"].Done())
                {
                    timer["JumpTimer"].Start(1.05f); // 점프 쿨타임
                    Jump();
                }
            }
            else if (currentState == AIState::Idle)
            {
                Idle();
            }

        }

        void Idle() override
        {
            if (currentState == AIState::CastingSkill) // 스킬 중에는 Idle 실행 방지
                return;
            animator.Play(&clips["idle"]);
        }

        void Move(Directions dir, float dt) 
        {
            if (currentState == AIState::CastingSkill) // 스킬 중에는 이동 실행 방지
                return;
            fz::Transform& transform = GetComponent<TransformComponent>();
          
            if (dir == Directions::RIGHT)
            {
                body->AddPosition({ MoveSpeed * 1.f, 0.0f });
                transform.SetScale(-1.0f, 1.0f);
                animator.Play(&clips["move"]);
            }
            else if (dir == Directions::LEFT)
            {
                body->AddPosition({ -MoveSpeed * 1.f, 0.0f }); //1->dt
                transform.SetScale(1.0f, 1.0f);
                animator.Play(&clips["move"]);
            }
        }

        void Jump() override
        {
            if (body->IsOnGround())
            {
                body->AddPosition({ 0.0f, JumpPower });
            }
        }

        void Damaged(int damage) override
        {
            animator.Play(&clips["damaged"]);
        }

        void Die() override
        {
            animator.Play(&clips["die"]);
        }
        void UseSkill()
        {
            if (!timer["SkillCooldown"].Done() || currentState == AIState::CastingSkill)
                return; // 쿨다운 중이면 실행하지 않음

            currentState = AIState::CastingSkill;

            animator.Play(&clips["skill1"]);
            animator2.Play(&clips2["skilleffect1"]);

            timer["SkillDuration"].Start(3.0f);
            // 3. 쿨다운 타이머 초기화
            timer["SkillCooldown"].Start(5.0f); // 5초 쿨타임
        }
    private:
        Directions MoveDirection = Directions::RIGHT;

        Directions GetRandomDirection()
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 1);
            return dis(gen) == 0 ? Directions::LEFT : Directions::RIGHT;
        }
    };

}
