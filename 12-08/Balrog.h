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
        AnimPool clips;

        Animator effectanimator;

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
        currentState = AIState::Moving;

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

            timer["ActionTimer"].Start(5.0f); //  ̵     ·      
            timer["SkillTimer"].Start(3.0f);
            timer["SkillCooldown"].Start(0.0f);

            // 각 스킬 쿨타임 설정
            timer["Skill1Cooldown"].Start(0.0f); // 초기화
            timer["Skill2Cooldown"].Start(0.0f);
            timer["Skill3Cooldown"].Start(0.0f);
        }

        void OnDestroy() override
        {
            FZLOG_DEBUG(" 발록스크립트 소멸 {0},{1}", 1.1, "aSDASCAsad");
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

            // 추적 상태
            if (currentState == AIState::Chasing)
            {
                FollowTarget(dt); // 거리 기반 추적
                return;
            }
            // 스킬 상태 처리 (추적과 분리)
            //if (currentState == AIState::Skill1)
            //{
            //    UseRandomSkill();
            //    return;
            //}


            if (timer["DamagedCooldown"].IsStart() && timer["DamagedCooldown"].Done())
            {
                currentState = AIState::Chasing;

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
                currentState = AIState::Die;
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
            if (status->Status == BalrogStatus::Skill1 ||
                status->Status == BalrogStatus::Skill2 ||
                status->Status == BalrogStatus::Skill3)
            {
                return; // 다른 스킬 실행 중이면 무시
            }

            animator.Play(&clips["skill1"]);
            status->Status = BalrogStatus::Skill1;
        }
        void Skill2() override
        {
            if (status->Status == BalrogStatus::Skill1 ||
                status->Status == BalrogStatus::Skill2 ||
                status->Status == BalrogStatus::Skill3)
            {
                return; // 다른 스킬 실행 중이면 무시
            }

            animator.Play(&clips["skill2"]);
            status->Status = BalrogStatus::Skill2;
        }
        void Skill3() override
        {
            if (status->Status == BalrogStatus::Skill1 ||
                status->Status == BalrogStatus::Skill2 ||
                status->Status == BalrogStatus::Skill3)
            {
                return; // 다른 스킬 실행 중이면 무시
            }

            animator.Play(&clips["skill3"]);
            status->Status = BalrogStatus::Skill3;
        }
        void UseRandomSkill()
        {
            if (!timer["SkillCooldown"].Done()) // 쿨타임 확인
                return;

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1, 3); // 1~3 랜덤 선택

            int randomSkill = dis(gen);

            switch (randomSkill)
            {
            case 1:
                Skill1();
                break;
            case 2:
                Skill2();
                break;
            case 3:
                Skill3();
                break;
            default:
                break;
            }

            timer["SkillCooldown"].Start(1.0f); // 쿨타임 1초 (필요하면 변경 가능)

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

            sf::Vector2f direction = targetPosition - myPosition;
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            if (distance > 400.0f)
            {
                currentState = AIState::Chasing;

                Directions dir = (targetPosition.x > myPosition.x) ? Directions::RIGHT : Directions::LEFT;
                float moveAmount = MoveSpeed * 1;

                if (dir == Directions::RIGHT)
                {
                    body->AddPosition({ moveAmount, 0.0f });
                    transform->Transform.SetScale(-1.0f, 1.0f);
      
                }
                else
                {
                    body->AddPosition({ -moveAmount, 0.0f });
                    transform->Transform.SetScale(1.0f, 1.0f);
     
                }
                if (animator.GetCurrentClip() != &clips["move"])
                {
                    animator.Play(&clips["move"]);
                }
            }
            else
            {
                if (animator.GetCurrentClip() != &clips["skill1"] && timer["SkillCooldown"].Done())
                {
            
                    UseRandomSkill(); // 랜덤 스킬 실행
                    timer["SkillCooldown"].Start(3.0f); // 스킬 쿨타임
                }
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