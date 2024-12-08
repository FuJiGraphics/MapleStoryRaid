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

        float KnockbackTime = 0.5f;

        Directions currDir = Directions::LEFT;

        Animator animator;
        AnimPool clips;

        TransformComponent* transform;
        RigidbodyComponent* body;
        StatComponent* stat = nullptr;
        TransformComponent* targetTransform = nullptr;

        bool isOnDie = false;
        Timer timer;

        enum class AIState {
            Idle, Moving, CastingSkill, Chasing, Die
        } currentState = AIState::Moving;

        void Start() override
        {

            stat = &AddComponent<StatComponent>();
            stat->Stat.HP = 2000;
            stat->Stat.MP = 0;
 

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


            body->SetGravityScale(1.5f);

            timer["ActionTimer"].Start(5.0f); // 이동 상태로 시작
            timer["JumpTimer"].Start(0.6f);   // 점프 딜레이
            timer["SkillJumpTimer"].Start(0.3f);   // 점프 딜레이

            timer["SkillCooldown"].Start(5.0f); // 스킬 쿨다운 초기화
            timer["SkillDuration"].Start(0.0f);
        }
        void OnDestroy() override
        {
            FZLOG_DEBUG("머쉬맘 스크립트 소멸");
        }
        void OnUpdate(float dt) override
        {
            if (!HasComponent<RigidbodyComponent>())
                return;

            auto& tagComp = GetComponent<TagComponent>();

            animator.Update(dt);
            timer.Update(dt);

            if (currentState == AIState::Die)
            {
                Die();
                if (timer["Die"].Done())
                {
                    GetCurrentScene()->DestroyInstance(GetCurrentEntity());
                }
                return;
            }

            if (currentState == AIState::CastingSkill)
            {
                if (!timer["SkillDuration"].IsStart())
                {
                    currentState = AIState::Chasing;
                }
                else if (timer["SkillDuration"].Done())
                {

                    timer["SkillDuration"].Start(1.05f); // 스킬 지속 시간
                    UseSkill(); // 스킬 사용
                    if (timer["SkillJumpTimer"].Done())
                    {
                        Jump();
                        timer["SkillJumpTimer"].Start(1.0); // 점프 쿨타임
                    }
                    currentState = AIState::Chasing; // 스킬 종료 후 Chasing 복귀
                }
                return;
            }

            if (timer["DamagedCooldown"].IsStart() && timer["DamagedCooldown"].Done())
            {
                currentState = AIState::Chasing;
            }

            if (currentState == AIState::Chasing)
            {
                if (!timer["SkillCooldown"].IsStart())
                {
                    timer["SkillCooldown"].Start(5.0f); // 쿨다운 타이머 시작
                }
                else if (timer["SkillCooldown"].Done())
                {
                    currentState = AIState::CastingSkill; // 스킬 상태로 전환
                    return; // 다른 로직 중단
                }
                FollowTarget(dt); // 추적 행동 수행

                if (timer["JumpTimer"].Done())
                {
                    Jump(); // 점프 실행
                    timer["JumpTimer"].Start(1.05f);
                }
                return;
            }

            if (timer["ActionTimer"].Done() && currentState != AIState::Chasing)
            {
                // 상태 전환
                if (currentState == AIState::Moving)
                {
                    currentState = AIState::Idle;
                    timer["ActionTimer"].Start(5.0f); // Idle 지속 시간
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
            else if (currentState == AIState::Die)
            {
                Die();
            }
            else
            {
                Idle();
            }

        }

        void Idle() override
        {
            //if (currentState == AIState::CastingSkill) // 스킬 중에는 Idle 실행 방지
            //    return;
            if (!timer["Knocback"].Done())
                return;
            animator.Play(&clips["idle"]);
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
                body->AddPosition({ -MoveSpeed * 1.f, 0.0f }); //1->dt
                transform.SetScale(1.0f, 1.0f);
                animator.Play(&clips["move"]);
            }
        }

        void Jump() override
        {
            if (body->IsOnGround({2.0f }))
            {
                body->AddPosition({ 0.0f, JumpPower });
            }
        }

        void Damaged(int damage) override
        {
            stat->Stat.HP -= damage;
            if (stat->Stat.HP <= 0)
            {
                stat->Stat.HP = 0;
                Die();
                currentState = AIState::Die;
                //return;
            }
            else
            {
                animator.Play(&clips["damaged"]);

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

        void Die() override
        {
            if (!timer["Die"].IsStart())
            {
                animator.Play(&clips["die"]);
                currentState = AIState::Die;
                timer["Die"].Start(1.f);
              /*  stat->Stat.IsDead = true;*/

             /* auto& callbackComp = GetComponent<CallbackComponent>();
                for (auto& fn : callbackComp.Callbacks["Die"])
                {
                   fn(GetCurrentEntity());
                }*/
            }
        }
        void UseSkill()
        {
            //currentState = AIState::CastingSkill;
            animator.Play(&clips["skill1"]);

        }

        void Knockback(Directions dir)
        {
            if (!timer["Knocback"].Done())
                return;
            timer["Knocback"].Start(KnockbackTime);
            if (dir == Directions::LEFT)
                body->AddForce({ -1000.f, -1000.0f });
            else if (dir == Directions::RIGHT)
                body->AddForce({ +1000.f, -1000.0f });
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
