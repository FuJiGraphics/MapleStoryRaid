#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"
#include <random>

namespace fz {

    class BalrogScript : public VegaScript, public MonsterFSM
    {
        using AnimPool = std::unordered_map<std::string, AnimationClip>;
    public:

        float MoveSpeed = 100.f;

        float KnockbackTime = 0.5f;
        Animator animator;
        AnimPool clips;

        TransformComponent* transform;
        RigidbodyComponent* body;
        StatComponent* stat;


        bool isOnDie = false;
        Timer timer;

        enum class AIState { Idle, Moving, Die } currentState = AIState::Moving;

        void Start() override
        {
            stat = &AddComponent<StatComponent>();
            stat->Stat.HP = 200;
            stat->Stat.MP = 0;
            stat->Stat.AD = 10;
            stat->Stat.AP = 0;
            transform = &GetComponent<TransformComponent>();
            body = &GetComponent<RigidbodyComponent>();

            sf::Sprite& sprite = GetComponent<SpriteComponent>();
            animator.SetTarget(GetCurrentEntity());
            animator.SetSpeed(1.0f);
            clips["idle"].loadFromFile("game/animations/balrog_idle.anim");
            clips["damaged"].loadFromFile("game/animations/balrog_damaged.anim");
            clips["move"].loadFromFile("game/animations/balrog_move.anim");
            clips["skill1"].loadFromFile("game/animations/balrog_skill1.anim");
            clips["skill1_e1"].loadFromFile("game/animations/balrog_skill1_effect1.anim");
            clips["skill1_e2"].loadFromFile("game/animations/balrog_skill1_effect2.anim");


            clips["skill2"].loadFromFile("game/animations/balrog_skill2.anim");
            clips["skill2_e1"].loadFromFile("game/animations/balrog_skill2_effect1.anim");
            clips["skill2_e2"].loadFromFile("game/animations/balrog_skill2_effect2.anim");
            clips["skill2_e3"].loadFromFile("game/animations/balrog_skill2_effect3.anim");

            clips["skill3"].loadFromFile("game/animations/balrog_skill3.anim");
            clips["skill3_e1"].loadFromFile("game/animations/balrog_skill3_effect1.anim");
            clips["skill3_e2"].loadFromFile("game/animations/balrog_skill3_effect2.anim");

            clips["die"].loadFromFile("game/animations/balrog_die.anim");
            body->SetGravityScale(1.5f);

            timer["ActionTimer"].Start(5.0f); // 이동 상태로 시작
        }
        void OnDestroy() override
        {
            FZLOG_DEBUG("주니어 발록 스크립트 소멸");
        }
        void OnUpdate(float dt) override
        {
            if (!HasComponent<RigidbodyComponent>())
                return;

            animator.Update(dt);

            // 이동 적용
            if (Input::IsKeyPressed(KeyType::D))
            {
                this->Move(Directions::RIGHT);
            }
            else if (Input::IsKeyPressed(KeyType::A))
            {
                this->Move(Directions::LEFT);
            }
            else if (Input::IsKeyPressed(KeyType::Q))
            {
                this->Damaged(0);
            }
            else if (Input::IsKeyPressed(KeyType::W))
            {
                this->Die();
            }
            else if (Input::IsKeyPressed(KeyType::E))
            {
                this->Skill1();
            }
            /*       else if (Input::IsKeyPressed(KeyType::R))
                   {
                       this->Skill2();
                   }*/
            else if (Input::IsKeyPressed(KeyType::T))
            {
                this->Skill3();
            }

            else
            {
                this->Idle();
            }
            // 점프 처리
            if (Input::IsKeyDown(KeyType::Space))
            {
                this->Jump();
            }


        }

        void Idle() override
        {
            animator.Play(&clips["idle"]);
        }
        void Move(Directions dir) override
        {
            fz::Transform& transform = GetComponent<TransformComponent>();
            // 이동 적용
            if (dir == Directions::RIGHT)
            {
                body->AddPosition({ MoveSpeed * 1.f, 0.0f });
                transform.SetScale(-1.0f, 1.0f);
                animator.Play(&clips["move"]);
            }
            else if (dir == Directions::LEFT)
            {
                body->AddPosition({ MoveSpeed * -1.f, 0.0f });
                transform.SetScale(1.0f, 1.0f);
                animator.Play(&clips["move"]);
            }
        }
        void Damaged(int damage) override
        {
            animator.Play(&clips["damaged"]);
        }

        void Skill1() override
        {
            animator.Play(&clips["skill1"]);
        }
        void Skill2() override
        {
            animator.Play(&clips["skill2"]);
        }
        void Skill3() override
        {
            animator.Play(&clips["skill3"]);
        }


        void Die() override
        {
            animator.Play(&clips["die"]);
        }

    private:

        Directions GetRandomDirection()
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 1);
            return dis(gen) == 0 ? Directions::LEFT : Directions::RIGHT;
        }
    };

}
