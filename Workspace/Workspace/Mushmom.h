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
        //float jumpCooldown = 1.05f; // 점프 쿨타임
        //float jumpDelay = 0.60f;
        //float jumpTimer = 0.0f;    // 점프 타이머
        //float actionTimer = 0.0f;  // 행동 타이머
        //float idleDuration = 3.0f; // 가만히 서있는 시간
        //float moveDuration = 5.0f; // 이동 시간

        float JumpPower = -430.f;
        float MoveSpeed = 100.f;

        Animator animator;
        AnimPool clips;

        TransformComponent* transform;
        RigidbodyComponent* body;

        Timer timer;

        enum class AIState { Idle, Moving } currentState = AIState::Moving;

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

            timer.Update(dt);

            if (timer["ActionTimer"].Done())
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

            if (currentState == AIState::Moving)
            {
                Move(MoveDirection, dt);

                if (timer["JumpTimer"].Done())
                {
                    timer["JumpTimer"].Start(1.05f); // 점프 쿨타임
                    Jump();
                }
            }
            else
            {
                Idle();
            }

        }

        void Idle() override
        {
            animator.Play(&clips["idle"]);
        }

        void Move(Directions dir, float dt) 
        {
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
            if (body->IsOnGround({ 0.0f, 2.0f }))
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
