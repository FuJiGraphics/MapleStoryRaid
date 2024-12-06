#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"
#include "BalrogStatus.hpp"
#include <random>
#include "Stat.hpp"

namespace fz {

    class BalrogScript : public VegaScript, public MonsterFSM
    {
        using AnimPool = std::unordered_map<std::string, AnimationClip>;
    public:

        float MoveSpeed = 100.f;
        float KnockbackTime = 0.5f;

        Directions currDir = Directions::LEFT;

        Animator animator;
        Animator effectanimator;

        AnimPool clips;

        TransformComponent* transform = nullptr;
        RigidbodyComponent* body = nullptr;
        StatComponent* stat = nullptr;
        TransformComponent* targetTransform = nullptr;

        BalrogStatusComponent* status = nullptr;
        GameObject CurrEffect;
        bool OnEffect = false;

        bool isOnDie = false;
        Timer timer;

        enum class AIState {
            Idle, Moving, Chasing,
            Die, Skill1,
            Skill2, Skill3
        }
        currentState = AIState::Idle;

        void Start() override
        {
            status = &AddComponent<BalrogStatusComponent>();

            stat = &AddComponent<StatComponent>();
            stat->Stat.HP = 200;
            stat->Stat.MP = 0;

            transform = &GetComponent<TransformComponent>();
            body = &GetComponent<RigidbodyComponent>();

            sf::Sprite& sprite = GetComponent<SpriteComponent>();

            animator.SetTarget(GetCurrentEntity());
            animator.SetSpeed(1.0f);
            effectanimator.SetTarget(GetCurrentEntity());
            effectanimator.SetSpeed(1.0f);
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
            timer["SkillTimer"].Start(3.0f);
        }

        void OnDestroy() override
        {
            FZLOG_DEBUG("주니어 발록 스크립트 소멸{0},{1}", 1.1, "aSDASCAsad");
        }

        void OnUpdate(float dt) override
        {
            if (!HasComponent<RigidbodyComponent>())
                return;

            animator.Update(dt);
            timer.Update(dt);

            if (currentState == AIState::Die)
            {
                Die(); // Die 상태 지속 처리
                if (timer["Die"].Done())
                {
                    GetCurrentScene()->DestroyInstance(GetCurrentEntity());
                }

                return;
            }

            if (currentState == AIState::Chasing && timer["SkillTimer"].Done())
            {
                // Chasing 상태에서만 Skill1으로 전환
                currentState = AIState::Skill1;
                timer["SkillTimer"].Start(1.0f); // Skill1은 1초 동안 유지
            }
            else if (currentState == AIState::Skill1 && timer["SkillTimer"].Done())
            {
                // Skill1 상태에서 Chasing으로 전환
                currentState = AIState::Chasing;
                timer["SkillTimer"].Start(3.0f); // Chasing은 3초 동안 유지
            }

            if (currentState == AIState::Chasing)
            {
                FollowTarget(dt);
                return;
            }

            if (currentState == AIState::Skill1)

            {
                Skill1();

            }
            if (!timer["Die"].IsStart() && timer["ActionTimer"].Done())
            {

                if (currentState == AIState::Moving)
                {
                    currentState = AIState::Idle;
                    timer["ActionTimer"].Start(3.0f);
                }
                else
                {
                    currentState = AIState::Moving;
                    timer["ActionTimer"].Start(5.0f);
                    MoveDirection = GetRandomDirection();
                }
            }

            if (currentState == AIState::Moving)
            {
                Move(MoveDirection, dt);
            }

            else if (currentState == AIState::Die)
            {
                Die();
                return;
            }
            else
            {
                Idle();
            }
        }
        void Effect()
        {
            if (!OnEffect)
            {
                OnEffect = true;

                const auto& scale = transform->Transform.GetScale();
                const auto& pos = GetWorldPosition();

                CurrEffect = GetCurrentScene()->Instantiate(
                    "BalrogSkill1", { pos.x - (30.f * scale.x), pos.y - 25.f }, scale);

                auto& effectStat = CurrEffect.AddComponent<StatComponent>();
                effectanimator.Play(&clips["skill1_e1"]);
            }
        }

        void Idle() override
        {
            if (!timer["Knocback"].Done())
                return;
            animator.Play(&clips["idle"]);
            status->Status = BalrogStatus::Idle;
        }

        void Move(Directions dir, float dt)
        {
            if (!timer["Knocback"].Done())
                return;

            fz::Transform& transform = GetComponent<TransformComponent>();

            currDir = dir;
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
            /*  status->Status = BalrogStatus::Move;   */
        }
        void Damaged(int damage) override
        {
            stat->Stat.HP -= damage;
            if (stat->Stat.HP <= 0)
            {
                stat->Stat.HP = 0;
                Die();
            }
            else
            {
                animator.Play(&clips["damaged"]);

                /*    status->Status = BalrogStatus::Damaged;*/
                GameObject player = GetCurrentScene()->GetEntityFromTag("Player");
                targetPlayer = player;

                if (currDir == Directions::LEFT)
                    Knockback(Directions::RIGHT);
                else if (currDir == Directions::RIGHT)
                    Knockback(Directions::LEFT);


                currentState = AIState::Chasing;
                currentState = AIState::Idle;
                timer["DamagedCooldown"].Start(1.0f);
            }
        }

        void Skill1() override
        {
            animator.Play(&clips["skill1"]);
            status->Status = BalrogStatus::Skill1;
        }
        void Skill2() override
        {
            animator.Play(&clips["skill2"]);
            status->Status = BalrogStatus::Skill2;
        }
        void Skill3() override
        {
            animator.Play(&clips["skill3"]);
            status->Status = BalrogStatus::Skill3;
        }


        void Die() override
        {
            if (!timer["Die"].IsStart())
            {
                animator.Play(&clips["die"]);
                currentState = AIState::Die;
                timer["Die"].Start(1.f);
                /*     status->Status = BalrogStatus::Die;*/
            }
        }
        void Knockback(Directions dir)
        {
            if (!timer["Knocback"].Done())
                return;
            timer["Knocback"].Start(KnockbackTime);
            if (dir == Directions::LEFT)
                body->AddForce({ -2000.f, -2000.0f });
            else if (dir == Directions::RIGHT)
                body->AddForce({ +2000.f, -2000.0f });
        }
        virtual void OnCollisionEnter(Collision collision)
        {
            if (collision.gameObject.HasComponent<SkillComponent>())
            {
                targetTransform = &collision.gameObject.GetComponent<TransformComponent>();

                const auto& stat = collision.gameObject.GetComponent<StatComponent>();
                Damaged(stat.Stat.AttackPower);
            }
        }

        void FollowTarget(float dt)
        {
            GameObject player = GetCurrentScene()->GetEntityFromTag("Player");
            auto targetPosition = player.GetComponent<TransformComponent>().Transform.GetTranslate();
            auto myPosition = transform->Transform.GetTranslate();

            Directions dir = (targetPosition.x > myPosition.x) ? Directions::RIGHT : Directions::LEFT;
            float moveAmount = MoveSpeed * dt;

            if (dir == Directions::RIGHT)
            {
                body->AddPosition({ MoveSpeed * 1, 0.0f });
                transform->Transform.SetScale(-1.0f, 1.0f);
                animator.Play(&clips["move"]);
            }
            else
            {
                body->AddPosition({ -MoveSpeed * 1, 0.0f });
                transform->Transform.SetScale(1.0f, 1.0f);
                animator.Play(&clips["move"]);
            }
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
        GameObject targetPlayer;
    };

}
