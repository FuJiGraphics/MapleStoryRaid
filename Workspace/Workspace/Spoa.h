#pragma once
#include <VegaEngine2.h>
#include "FSM.h"
#include "Utils/Timer.h"
#include <random>

namespace fz {

    class SpoaScript : public VegaScript, public MonsterFSM
    {
        using AnimPool = std::unordered_map<std::string, AnimationClip>;
    public:
        float JumpPower = -500.f;
        float MoveSpeed = 100.f;

        float KnockbackTime = 0.5f;

        Directions currDir = Directions::LEFT;

        Animator animator;
        AnimPool clips;

        TransformComponent* transform = nullptr;
        RigidbodyComponent* body = nullptr;
        StatComponent* stat = nullptr;
        TransformComponent* targetTransform = nullptr;

        bool isOnDie = false;
        Timer timer;
        int spawnCount = 0;
        enum class AIState { Idle, Moving, Chasing, Die } currentState = AIState::Moving;

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
            clips["idle"].loadFromFile("game/animations/spoa_idle.anim");
            clips["move"].loadFromFile("game/animations/spoa_move.anim");
            clips["damaged"].loadFromFile("game/animations/spoa_damaged.anim");
            clips["die"].loadFromFile("game/animations/spoa_die.anim");

            body->SetGravityScale(1.5f);

            timer["ActionTimer"].Start(5.0f); // 이동 상태로 시작
        }

        void OnDestroy() override
        {
            FZLOG_DEBUG("스포아 스크립트 파괴!{0} { 1 }", 1.1, "aSDASCAsad");
        }

        void OnUpdate(float dt) override
        {
            if (!HasComponent<RigidbodyComponent>())
                return;

            animator.Update(dt);
            timer.Update(dt);

            if (timer["Die"].IsStart() && timer["Die"].Done())
            {
                GetCurrentScene()->DestroyInstance(GetCurrentEntity());
                return;

            }
            else if (timer["Die"].IsStart())
            {
                Die();
                return;
            }
            if (timer["DamagedCooldown"].IsStart() && timer["DamagedCooldown"].Done())
            {
                currentState = AIState::Chasing;
            }
            if (currentState == AIState::Chasing)
            {
                FollowTarget(dt); // 추적 로직
                return;
            }

            if (!timer["Die"].IsStart() && timer["ActionTimer"].Done())
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

        void Idle() override
        {
            if (!timer["Knocback"].Done())
                return;
            animator.Play(&clips["idle"]);
        }

        void Move(Directions dir, float dt)
        {
            if (!timer["Knocback"].Done())
                return;
            fz::Transform& transform = GetComponent<TransformComponent>();
            // 이동 적용
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

                GameObject player = GetCurrentScene()->GetEntityFromTag("Player");
                targetPlayer = player;  // 맞은 스포아만 추적 타겟 설정

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
            animator.Play(&clips["die"]);
            currentState = AIState::Die;
            timer["Die"].Start(0.5f);
            stat->Stat.IsDead = true;

            spawnCount--;
            Spawn();

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
                Damaged(stat.Stat.AD);
            }
        }
        void FollowTarget(float dt)
        {
            // 추적 대상의 위치
            GameObject player = GetCurrentScene()->GetEntityFromTag("Player");
            auto targetPosition = player.GetComponent<TransformComponent>().Transform.GetTranslate();
            auto myPosition = transform->Transform.GetTranslate();

            GameObject line1 = GetCurrentScene()->GetEntityFromTag("Line1");
            auto linePosition1 = line1.GetComponent<TransformComponent>().Transform.GetTranslate();

            GameObject line2 = GetCurrentScene()->GetEntityFromTag("Line2");
            auto linePosition2 = line2.GetComponent<TransformComponent>().Transform.GetTranslate();

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

        void Spawn() override
        {
            static int spawnCount = 0; // 스폰된 몬스터 수를 추적하는 변수

            // 최대 스폰 제한을 넘지 않도록 제어
            if (spawnCount >= maxSpawnCount)
            {
                return;
            }
            std::vector<std::string> spawnLines = { "Line1", "Line2", "Line3" };

            // 랜덤으로 하나의 Line 선택
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> randomIndex(0, spawnLines.size() - 1);
            std::string selectedLine = spawnLines[randomIndex(gen)];

            // 선택된 Line 엔티티 가져오기
            GameObject line = GetCurrentScene()->GetEntityFromTag(selectedLine);
            auto& edgeCollider = line.GetComponent<EdgeCollider2DComponent>();
            auto& transform = line.GetComponent<TransformComponent>().Transform;

            // StartPos와 EndPos를 기반으로 스폰 위치 계산
            sf::Vector2f startWorldPos = edgeCollider.GetStartPos();
            sf::Vector2f endWorldPos = edgeCollider.GetEndPos();

            std::uniform_real_distribution<float> randomX(startWorldPos.x, endWorldPos.x);
            float spawnX = randomX(gen);
            float spawnY = startWorldPos.y;

            // 스폰 위치 설정
            sf::Vector2f spawnPosition = { spawnX, spawnY };

            // 스포아 생성
            GetCurrentScene()->Instantiate("Spoa", spawnPosition);
            spawnCount++;
        }


    private:

        const int maxSpawnCount = 3;

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
